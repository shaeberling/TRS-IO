#ifndef __WEB_DEBUGGER_H__
#define __WEB_DEBUGGER_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint16_t af;
  uint16_t bc;
  uint16_t de;
  uint16_t hl;
  uint16_t af_prime;
  uint16_t bc_prime;
  uint16_t de_prime;
  uint16_t hl_prime;

  uint16_t pc;
  uint16_t sp;
  uint16_t ix;
  uint16_t iy;

  uint8_t i;  /* interrupt-page address register */
  uint8_t r;  /* memory-refresh register */
  uint8_t r7; /* bit 7 of refresh register saved */

  uint8_t iff1;
  uint8_t iff2;

  uint64_t t_count;
  float clock_mhz;
  uint8_t interrupt_mode;
} TRX_StatusRegistersAndFlags;

typedef enum {
  TRX_SCREEN_NORMAL = 0,
  TRX_SCREEN_EXPANDED = 1,
  TRX_SCREEN_ALTERNATE = 2,
  TRX_SCREEN_INVERTED = 3
} TRX_ScreenMode;

typedef struct {
  bool paused;
  TRX_StatusRegistersAndFlags registers;
  TRX_ScreenMode screen_mode;
} TRX_SystemState;

typedef enum {
  UNDEFINED = 0,
  MODEL_I = 1,
  MODEL_II = 2,
  MODEL_III = 3,
  MODEL_IV = 4,
  MODEL_IV_P = 5,
} TRX_ModelType;

typedef struct {
  uint32_t start;
  uint32_t length;
} TRX_MemoryRange;

typedef struct {
  bool control_step;
  bool control_step_over;
  bool control_continue;
  bool control_pause;
  bool pc_breakpoints;
  bool memory_breakpoints;
  bool io_breakpoints;
  TRX_MemoryRange memory_range;
  // TOOD: Supported Memory Segment. (A single segment should suffice)
  //       Model 1 demo will only work on address 32k and up.
  //       (For read and write)
} TRX_Capabilities;

typedef void (*TRX_UpdateState)(TRX_SystemState* state);

typedef enum {
  TRX_CONTROL_TYPE_NOOP = 0,
  TRX_CONTROL_TYPE_STEP = 1,
  TRX_CONTROL_TYPE_STEP_OVER = 2,
  // Note: This is called asynchronously so doesn't need to return immediately.
  TRX_CONTROL_TYPE_CONTINUE = 3,
  TRX_CONTROL_TYPE_HALT = 4,
  TRX_CONTROL_TYPE_PAUSE = 5,
  TRX_CONTROL_TYPE_SOFT_RESET = 6,
  TRX_CONTROL_TYPE_HARD_RESET = 7
} TRX_CONTROL_TYPE;
typedef void (*TRX_ControlCallback)(TRX_CONTROL_TYPE type);

typedef enum {
  TRX_BREAK_PC = 0,
  TRX_BREAK_MEMORY = 1,
  TRX_BREAK_IO = 2
} TRX_BREAK_TYPE;
typedef void (*TRX_SetBreakPointCallback)(int bp_id, uint16_t addr, TRX_BREAK_TYPE type);
typedef void (*TRX_RemoveBreakPointCallback)(int bp_id);

typedef struct {
  TRX_MemoryRange range;
  uint8_t* data;
} TRX_MemorySegment;
typedef uint8_t (*TRX_MemoryRead)(uint16_t addr);
typedef void (*TRX_MemoryWrite)(uint16_t addr, uint8_t value);

typedef enum {
  TRX_RES_MAIN_HTML = 0,
  TRX_RES_MAIN_JS = 1,
  TRX_RES_MAIN_CSS = 2,
  TRX_RES_TRS_FONT = 3,
  TRX_RES_JQUERY = 4,
} TRX_RESOURCE_TYPE;
typedef char* (*TRX_GetResource)(TRX_RESOURCE_TYPE type);

typedef void (*TRX_OnPokeMemory)(uint16_t addr, uint8_t value, void* clazz);
typedef void (*TRX_RegisterCallbacks)(void* clazz, TRX_OnPokeMemory opm);

typedef void (*TRX_KeyEvent)(const char* key, bool down, bool shift);

typedef struct {
  // Descriptive name of the system under test (SUT).
  const char* system_name;

  // TRS model type.
  TRX_ModelType model;

  // Original = 0 or 1.
  uint8_t rom_version;

  // Lets the frontend know about the SUT's capabilities.
  TRX_Capabilities capabilities;

  // Callbacks invoked from the frontend.
  TRX_ControlCallback control_callback;
  TRX_SetBreakPointCallback breakpoint_callback;
  TRX_RemoveBreakPointCallback remove_breakpoint_callback;
  TRX_MemoryRead read_memory;
  TRX_MemoryWrite write_memory;
  TRX_GetResource get_resource;
  TRX_KeyEvent key_event;

  TRX_RegisterCallbacks register_callbacks;

  // Tells the frontend about state changes of the SUT.
  TRX_UpdateState get_state_update;

} TRX_Context;

class WebDebugger {
 public:
  explicit WebDebugger();

  // Initialize the web debugger.
  bool init_trs_xray(TRX_Context* ctx);

  // Initialize the web debugger.
  void trx_waitForExit();

  // Initialize the web debugger.
  void trx_shutdown();

  // Let WebDebugger handle a web request. Will return wether it handled it.
  bool handle_www(struct mg_connection *conn,
                  int ev, void *ev_data, void *fn_data);

  void handle_dynamic_update();

  private:
   void send_update_to_web_debugger();
   void send_memory_segment(const char* params);
   void add_breakpoint(const char* params, TRX_BREAK_TYPE type);
   void remove_breakpoint(const char* params);
   void key_event(const char* params);

   bool handle_http_request(struct mg_connection *conn,
                            struct mg_http_message* message,
                            bool throw_404);
   char* get_registers_json(const TRX_StatusRegistersAndFlags* regs);
   void get_memory_segment(int start, int length, TRX_MemorySegment* segment,
                           bool force_full_update);
   void on_poke_memory(uint16_t addr, uint8_t value);
   static void on_poke_memory_s(uint16_t addr, uint8_t value, void* clazz);

   int emu_run_looper(void *ptr);
   void on_frontend_message(const char* msg);
   bool www_handler(struct mg_connection *conn,
                    int ev, void *ev_data, void *fn_data, bool throw_404);
   void http_reply(struct mg_connection *c, int code, const char *headers,
                   const char *msg);
   unsigned long get_millis();

   TRX_Context* ctx_;
   TRX_MemorySegment memory_query_cache_;
   unsigned long last_update_sent_;

   // The range of memory that contains changes since the last update.
   uint16_t changed_mem_range_start_;
   uint16_t changed_mem_range_end_;
};



#endif  // __WEB_DEBUGGER_H__

// Questions:
// - When are the different opcodes being used? http://z80-heaven.wikidot.com/opcode-reference-chart
//   - minor: CB, CC, ED, FD, DDCB, FDCB etc.
