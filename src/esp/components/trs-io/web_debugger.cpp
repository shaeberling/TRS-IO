#include "web_debugger.h"

#include "esp_log.h"
#include <stdlib.h>

#include "cJSON.h"
#include "mongoose.h"
// #include "trs.h"


static const char* TAG = "TRX";

// static struct mg_mgr www_mgr;
// static SDL_Thread* thread;
// static SDL_Thread* emu_run_thread;
static struct mg_connection* status_conn = NULL;
static bool trx_running = false;

static bool emulation_running = false;
static int emulation_is_halting = false;
static uint32_t last_update_sent = 0;

typedef struct {
  uint16_t address;
  TRX_BREAK_TYPE type;
  bool enabled;
} TRX_Breakpoint;

#define MAX_BREAKPOINTS 128
static TRX_Breakpoint breakpoints[MAX_BREAKPOINTS];

static TRX_CONTROL_TYPE next_async_action = TRX_CONTROL_TYPE_NOOP;


WebDebugger::WebDebugger() : ctx_(NULL),
                             last_update_sent_(0),
                             changed_mem_range_start_(0xFFFF),
                             changed_mem_range_end_(0) {
  // Pre-allocate for performance to max required size.
  memory_query_cache_.data = (uint8_t*) malloc(sizeof(uint8_t) * 0xFFFF);
}

// public
bool WebDebugger::init_trs_xray(TRX_Context* ctx_param) {
  // if (!init_webserver()) {
  //   puts("[TRX] ERROR: Aborting initialization.");
  //   return false;
  // }
  last_update_sent = clock();
  ctx_ = ctx_param;

  for (int id = 0; id < MAX_BREAKPOINTS; ++id) {
    breakpoints[id].address = 0;
    breakpoints[id].type = TRX_BREAK_PC;
    breakpoints[id].enabled = false;
  }

  ctx_->register_callbacks(this, &WebDebugger::on_poke_memory_s);

  trx_running = true;
  // emu_run_thread =
  //     SDL_CreateThread(emu_run_looper, "TRX Emu Run Thread", (void *)NULL);
  return true;
}

// public
void WebDebugger::trx_waitForExit() {
  int threadReturnValue;
  // SDL_WaitThread(thread, &threadReturnValue);
}

// public
void WebDebugger::trx_shutdown() {
  puts("[TRX] Shutting down Web debugger");
  trx_running = false;
}

// public
bool WebDebugger::handle_www(struct mg_connection *conn,
                             int ev, void *ev_data, void *fn_data) {
  // Simply forward to the internal implementation, but disable throwing a
  // 404 if a resource is not found. We'll let the caller handle that case.
  return this->www_handler(conn, ev, ev_data, fn_data, /*throw_404= */ false);
}

void WebDebugger::send_update_to_web_debugger() {
  if (status_conn == NULL) return;

  TRX_SystemState state;
  ctx_->get_state_update(&state);

  // Send registers.
  char* message = get_registers_json(&state.registers);
  mg_ws_send(status_conn, message, strlen(message), WEBSOCKET_OP_TEXT);
  free(message);
}

// public
void WebDebugger::handle_dynamic_update() {
  // We want to send one last update when the running emulation shut down so
  // that the frontend has the latest state.
  if (!emulation_is_halting && !emulation_running) return;

  if (emulation_is_halting) {
    ESP_LOGI(TAG, "Halting emulation; sending one more update.");
  }
  auto now_millis = get_millis();
  auto diff_millis = now_millis - last_update_sent_;

  if (diff_millis < 300 && !emulation_is_halting) return;

  ESP_LOGI(TAG, "Pre send update");

  send_update_to_web_debugger();
  // Note that the actual memory segment being sent should only be the range
  // that changed.
  ESP_LOGI(TAG, "PRE  send memory update");
  send_memory_segment("0/65536");
  ESP_LOGI(TAG, "POST send memory update");
  last_update_sent_ = now_millis;
  emulation_is_halting = false;
}

// Params: [start]/[length], e.g. "0/65536"
void WebDebugger::send_memory_segment(const char* params) {
  if (status_conn == NULL) return;
  // printf("TRX: MemorySegment request: '%s'.", params);

  long start = get_millis();
  int param_start = 0;
  int param_length = 0xFFFF;

  bool force_update = strcmp("force_update", params) == 0;

  if (!force_update) {
    // Extract parameters
    int delim_pos = strchr(params, '/') - params;
    char param_start_str[delim_pos + 1];
    memcpy(param_start_str, params, delim_pos );
    param_start_str[delim_pos] = '\0';
    param_start = atoi(param_start_str);

    int substr_length = strlen(params) - delim_pos;
    char param_length_str[substr_length];
    memcpy(param_length_str, params + delim_pos + 1, substr_length - 1);
    param_length_str[substr_length - 1] = '\0';
    param_length = atoi(param_length_str);
    // printf("Parameters: start(%d) length(%d)\n", param_start, param_length);
  }

  long t_gms = get_millis();
  get_memory_segment(param_start, param_length, &memory_query_cache_,
                     force_update);
  long t_gms_diff = get_millis() - t_gms;
  printf("[TRX] 'get_memory_segment' took %ld millis\n", t_gms_diff);

  const TRX_MemorySegment* seg = &memory_query_cache_;

  printf("[TRX] Segment: start: %d, length: %d\n", seg->range.start, seg->range.length);

  // // Add start metadata.
  uint8_t* data_to_send = (uint8_t*) malloc(sizeof(uint8_t) * (seg->range.length + 2));
  uint8_t param_start_1 = (seg->range.start & 0xFF00) >> 8;
  uint8_t param_start_2 = seg->range.start & 0x00FF;
  data_to_send[0] = param_start_1;
  data_to_send[1] = param_start_2;
  // printf("Start param pieces: %d %d\n", param_start_1, param_start_2);
  memcpy(data_to_send + 2, seg->data, seg->range.length);

  long t_send = get_millis();
  // Send registers.
  mg_ws_send(status_conn, (const char*)data_to_send, seg->range.length + 2, WEBSOCKET_OP_BINARY);
  free(data_to_send);
  long t_send_diff = get_millis() - t_send;
  printf("[TRX] 'mg_ws_send' took %ld millis\n", t_send_diff);

  long diff = get_millis() - start;
  printf("[TRX] 'send_memory_segment' took %ld millis\n", diff);
}

unsigned long WebDebugger::get_millis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

// Params: [address in decimal]. e.g. "1254"
void WebDebugger::add_breakpoint(const char* params, TRX_BREAK_TYPE type) {
  int addr = atoi(params);
  if (addr == 0 && strcmp("0", params) != 0) {
    puts("[TRX] Error: Cannot parse address.");
    return;
  }

  int id = 0;
  for (id = 0; id < MAX_BREAKPOINTS; ++id) {
    if (!breakpoints[id].enabled) break;
  }
  breakpoints[id].address = addr;
  breakpoints[id].type = type;
  breakpoints[id].enabled = true;
  ctx_->breakpoint_callback(id, addr, type);
  send_update_to_web_debugger();
}

void WebDebugger::remove_breakpoint(const char* params) {
  int id = atoi(params);
  if (id == 0 && strcmp("0", params) != 0) {
    puts("[TRX] Error: Cannot parse breakpoint ID.");
    return;
  }
  if (id >= MAX_BREAKPOINTS) {
    puts("[TRX] Error: Breakpoint ID too large.");
    return;
  }
  breakpoints[id].enabled = false;
  ctx_->remove_breakpoint_callback(id);
  send_update_to_web_debugger();
}

// Param: [1=down,0=up]/[1=shift,0=no shift]/[key]
void WebDebugger::key_event(const char* params) {
  bool down = params[0] == '1';
  bool shift = params[2] == '1';
  const char* key = params + 4;
  ctx_->key_event(key, down, shift);
}

bool WebDebugger::handle_http_request(struct mg_connection *conn,
                                      struct mg_http_message* message,
                                      bool throw_404) {
  if (!trx_running) return false;
  if (mg_http_match_uri(message, "/debugger")) {
    http_reply(conn, 200, "Content-Type: text/html\r\nConnection: close\r\n",
               ctx_->get_resource(TRX_RES_MAIN_HTML));
  } else if (mg_http_match_uri(message, "/web_debugger.js")) {
    http_reply(conn, 200, "Content-Type: application/javascript\r\nConnection: close\r\n",
               ctx_->get_resource(TRX_RES_MAIN_JS));
  } else if (mg_http_match_uri(message, "/web_debugger.css")) {
    http_reply(conn, 200, "Content-Type: text/css\r\nConnection: close\r\n",
               ctx_->get_resource(TRX_RES_MAIN_CSS));
  } else if (mg_http_match_uri(message, "/channel")) {
    ESP_LOGI(TAG, "Upgrading WebSocket");
		mg_ws_upgrade(conn, message, NULL);
		status_conn = conn;
  } else {
    // Resource not found.
    if (throw_404) {
      http_reply(conn, 404, "Content-Type: text/html\r\nConnection: close\r\n", "");
    }
    return false;
  }
  return true;
}

void WebDebugger::http_reply(struct mg_connection *c, int code,
                             const char *headers, const char *msg) {
  int len = strlen(msg);
  mg_printf(c, "HTTP/1.1 %d OK\r\n%sContent-Length: %d\r\n\r\n", code,
            headers == NULL ? "" : headers, len);
  mg_send(c, msg, len);
}

char* WebDebugger::get_registers_json(const TRX_StatusRegistersAndFlags* regs) {
  cJSON* json = cJSON_CreateObject();
  if (!trx_running) {
    char* str = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    return str;
  }

  cJSON* context = cJSON_CreateObject();
  cJSON_AddStringToObject(context, "system_name", ctx_->system_name);
  cJSON_AddNumberToObject(context, "model", ctx_->model);
  cJSON_AddItemToObject(json, "context", context);

  cJSON* breaks = cJSON_CreateArray();
  for (int i = 0; i < MAX_BREAKPOINTS; ++i) {
    if (!breakpoints[i].enabled) continue;
    cJSON* breakpoint = cJSON_CreateObject();
    cJSON_AddNumberToObject(breakpoint, "id", i);
    cJSON_AddNumberToObject(breakpoint, "address", breakpoints[i].address);
    cJSON_AddNumberToObject(breakpoint, "type", breakpoints[i].type);
    cJSON_AddItemToArray(breaks, breakpoint);
  }
  cJSON_AddItemToObject(json, "breakpoints", breaks);

  cJSON* registers = cJSON_CreateObject();
  cJSON_AddNumberToObject(registers, "pc", regs->pc);
  cJSON_AddNumberToObject(registers, "sp", regs->sp);
  cJSON_AddNumberToObject(registers, "af", regs->af);
  cJSON_AddNumberToObject(registers, "bc", regs->bc);
  cJSON_AddNumberToObject(registers, "de", regs->de);
  cJSON_AddNumberToObject(registers, "hl", regs->hl);
  cJSON_AddNumberToObject(registers, "af_prime", regs->af_prime);
  cJSON_AddNumberToObject(registers, "bc_prime", regs->bc_prime);
  cJSON_AddNumberToObject(registers, "de_prime", regs->de_prime);
  cJSON_AddNumberToObject(registers, "hl_prime", regs->hl_prime);
  cJSON_AddNumberToObject(registers, "ix", regs->ix);
  cJSON_AddNumberToObject(registers, "iy", regs->iy);
  cJSON_AddNumberToObject(registers, "i", regs->i);
  cJSON_AddNumberToObject(registers, "r_1", regs->r);
  cJSON_AddNumberToObject(registers, "r_2", (regs->r7 & 0x7f));

  cJSON_AddNumberToObject(registers, "z80_t_state_counter", regs->t_count);
  cJSON_AddNumberToObject(registers, "z80_clockspeed", regs->clock_mhz);
  cJSON_AddNumberToObject(registers, "z80_iff1", regs->iff1);
  cJSON_AddNumberToObject(registers, "z80_iff2", regs->iff2);
  cJSON_AddNumberToObject(registers, "z80_interrupt_mode", regs->interrupt_mode);

  cJSON_AddItemToObject(json, "registers", registers);

  char* str = cJSON_PrintUnformatted(json);
  cJSON_Delete(json);
  return str;
}

void WebDebugger::get_memory_segment(int start, int length,
                                     TRX_MemorySegment* segment,
                                     bool force_full_update) {
  uint16_t mem_changed_start = start > changed_mem_range_start_ ?
                               start : changed_mem_range_start_;
  uint16_t mem_changed_end = start + length < changed_mem_range_end_ ?
                             start + length : changed_mem_range_end_;

  // Reset the range.
  changed_mem_range_start_ = 0xFFFF;
  changed_mem_range_end_ = 0;

  int mem_changed_length = mem_changed_end - mem_changed_start;
  if (mem_changed_length < 0) mem_changed_length = 0;

	int start_actual = start;
  if (!force_full_update) {
    start_actual = mem_changed_start;
  }

	int length_actual = length;
  if (!force_full_update) {
    length_actual = mem_changed_length;
  }
	segment->range.start = start_actual;
	segment->range.length = length_actual;
	for (int i = start_actual; i < start_actual + length_actual; ++i) {
		int data = ctx_->read_memory(i);
		segment->data[i - start_actual] = data;
	}
}

void WebDebugger::on_poke_memory(uint16_t address, uint8_t value) {
  // Update changed range.
  if (address < changed_mem_range_start_) changed_mem_range_start_ = address;
  if (address > changed_mem_range_end_) changed_mem_range_end_ = address;
}

// static
void WebDebugger::on_poke_memory_s(uint16_t addr, uint8_t value, void* clazz) {
  static_cast<WebDebugger*>(clazz)->on_poke_memory(addr, value);
}

// Waits until it gets the signal to initiate continuous running.
int WebDebugger::emu_run_looper(void *ptr) {
  while(trx_running) {
    // SDL_Delay(50);  FIXME
    if (next_async_action != TRX_CONTROL_TYPE_NOOP) {
      emulation_running = true;
      ctx_->control_callback(next_async_action);
      emulation_running = false;
      emulation_is_halting = true;
      next_async_action = TRX_CONTROL_TYPE_NOOP;
    }
  }
  return 0;
}

void WebDebugger::on_frontend_message(const char* msg) {
  // ESP_LOGI(TAG, "Frontend message: %s", msg);
  if (!trx_running) return;
  if (strcmp("action/refresh", msg) == 0) {
    send_update_to_web_debugger();
  } else if (strcmp("action/step", msg) == 0) {
    ctx_->control_callback(TRX_CONTROL_TYPE_STEP);
    send_update_to_web_debugger();
  } else if (strcmp("action/step-over", msg) == 0) {
    ctx_->control_callback(TRX_CONTROL_TYPE_STEP_OVER);
    send_update_to_web_debugger();
  } else if (strcmp("action/continue", msg) == 0) {
    // Note: Doesn't block here on PocketTRS.
    ctx_->control_callback(TRX_CONTROL_TYPE_CONTINUE);
    emulation_running = true;
  } else if (strcmp("action/stop", msg) == 0) {
    ctx_->control_callback(TRX_CONTROL_TYPE_HALT);
    emulation_running = false;
    emulation_is_halting = true;
    send_update_to_web_debugger();
  } else if (strcmp("action/pause", msg) == 0) {
    ctx_->control_callback(TRX_CONTROL_TYPE_PAUSE);
    send_update_to_web_debugger();
  } else if (strcmp("action/soft_reset", msg) == 0) {
    ctx_->control_callback(TRX_CONTROL_TYPE_SOFT_RESET);
  } else if (strcmp("action/hard_reset", msg) == 0) {
    ctx_->control_callback(TRX_CONTROL_TYPE_HARD_RESET);
  } else if (strncmp("action/get_memory", msg, 17) == 0) {
    send_memory_segment(msg + 18);
  } else if (strncmp("action/add_breakpoint/pc", msg, 24) == 0) {
    add_breakpoint(msg + 25, TRX_BREAK_PC);
  } else if (strncmp("action/add_breakpoint/mem", msg, 25) == 0) {
    add_breakpoint(msg + 26, TRX_BREAK_MEMORY);
  } else if (strncmp("action/remove_breakpoint", msg, 24) == 0) {
    remove_breakpoint(msg + 25);
  } else if (strncmp("action/key_event", msg, 16) == 0) {
    key_event(msg + 17);
  } else {
    printf("[TRX] WARNING: Unknown message: '%s'\n", msg);
  }
}

bool WebDebugger::www_handler(struct mg_connection *conn,
                              int ev, void *ev_data, void *fn_data, bool throw_404 = true) {
  if (!trx_running) {
    return false;
  }
  switch(ev) {
    case MG_EV_HTTP_MSG: {
      return handle_http_request(conn, (struct mg_http_message*) ev_data, throw_404);
    }
    case MG_EV_WS_MSG: {
      // ESP_LOGI(TAG, "www_handler -> MG_EV_WS_MSG");
      static char message[50];
      struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
      strncpy(message, wm->data.ptr, wm->data.len);
      message[wm->data.len] = '\0';
      on_frontend_message(message);
      break;
    }
    case MG_EV_CLOSE: {
      if (conn == status_conn) {
        ESP_LOGI(TAG, "www_handler -> WS  MG_EV_CLOSE");
        status_conn = NULL;
      }
      break;
    }
  }
  return true;
}

// static int www_looper(void *ptr) {
//   while (trx_running) {
//     mg_mgr_poll(&www_mgr, 40);
//     handleDynamicUpdate();
//   }
//   mg_mgr_free(&www_mgr);
//   return 0;
// }

// static bool init_webserver(void) {
//   mg_mgr_init(&www_mgr);
//   struct mg_connection *conn = mg_http_listen(
//       &www_mgr, "0.0.0.0:8080", &www_handler, NULL /*fn_data args */);
//   if (conn == NULL) {
//     puts("[TRX] ERROR(Mongoose): Cannot set up listener!");
//     return false;
//   }
//   thread = SDL_CreateThread(www_looper, "Mongoose thread", (void *)NULL);
//   if (thread == NULL) {
//     puts("[TRX] ERROR(Mongoose/SDL): Unable to create thread!");
//     return false;
//   }
//   return true;
// }

