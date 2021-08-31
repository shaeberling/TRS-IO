//
// Created by Sascha Häberling sascha.codes in 2021.
//

#include "trs_virtual_interface.h"

#include <functional>
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "fabgl.h"
#include "trs-keyboard.h"
#include <map>

static const char* TAG = "TRS-VInterface";

#define DATA_UPDATE_DELAY_MICROS 150

// Message type for the interface (1 byte).
#define MSG_SCREEN_UPDATE 10
#define MSG_PRINTER_WRITE 20
#define MSG_PRINTER_NEW_LINE 21

typedef std::pair<std::string, VirtualKey> KeyMapPair;

// public static
TrsVirtualInterface* TrsVirtualInterface::instance() {
  if (instance_ == NULL) {
    instance_ = new TrsVirtualInterface();
  }
  return instance_;
}

TrsVirtualInterface::TrsVirtualInterface() : keyboard_(NULL),
                                             dataChannelSem_(NULL),
                                             frontendConnected_(false),
                                             screenBuffer_(NULL) {
  setChannelClosed();
}

void TrsVirtualInterface::init(fabgl::Keyboard* kb) {
  keyboard_ = kb;
  dataChannelSem_ = xSemaphoreCreateMutex();

  // Create and start task that periodically sends updates to the frontend.
  std::string task_name = "vi_screen_updater";
  auto rt = xTaskCreatePinnedToCore(
      TrsVirtualInterface::startUpdateLoop,
      task_name.c_str(), 6000, (void*) this, 1, NULL, 0);
  if (rt != pdPASS) {
    ESP_LOGE(TAG, "Failed to create frontend update task");
  } else {
    ESP_LOGI(TAG, "Frontend updater task initialized");
  }

  // Initialize ASCII to FabGLs VK mapping.
  for (const auto vk : allKeys) {
    int ascii = kb->virtualKeyToASCII(vk);
    if (ascii) {
      auto key = std::string(1, static_cast<char>(ascii));
      asciiToVK_.insert(KeyMapPair(key, vk));
    }
  }

  // Add non-standard key mappings.
  asciiToVK_.insert(KeyMapPair("ArrowLeft", fabgl::VK_LEFT));
  asciiToVK_.insert(KeyMapPair("ArrowRight", fabgl::VK_RIGHT));
  asciiToVK_.insert(KeyMapPair("ArrowUp", fabgl::VK_UP));
  asciiToVK_.insert(KeyMapPair("ArrowDown", fabgl::VK_DOWN));
  asciiToVK_.insert(KeyMapPair("Enter", fabgl::VK_RETURN));
  asciiToVK_.insert(KeyMapPair("Tab", fabgl::VK_TAB));
  asciiToVK_.insert(KeyMapPair("F1", fabgl::VK_F1));
  asciiToVK_.insert(KeyMapPair("F2", fabgl::VK_F2));
  asciiToVK_.insert(KeyMapPair("F3", fabgl::VK_F3));
  asciiToVK_.insert(KeyMapPair("F4", fabgl::VK_F4));
  asciiToVK_.insert(KeyMapPair("F5", fabgl::VK_F5));
  asciiToVK_.insert(KeyMapPair("F6", fabgl::VK_F6));
  asciiToVK_.insert(KeyMapPair("F7", fabgl::VK_F7));
  asciiToVK_.insert(KeyMapPair("F8", fabgl::VK_F8));
  asciiToVK_.insert(KeyMapPair("F9", fabgl::VK_F9));
  asciiToVK_.insert(KeyMapPair("F10", fabgl::VK_F10));
  asciiToVK_.insert(KeyMapPair("F11", fabgl::VK_F11));
  asciiToVK_.insert(KeyMapPair("F12", fabgl::VK_F12));
  asciiToVK_.insert(KeyMapPair("Escape", fabgl::VK_ESCAPE));
  asciiToVK_.insert(KeyMapPair("%20", fabgl::VK_SPACE));
  asciiToVK_.insert(KeyMapPair("Backspace", fabgl::VK_BACKSPACE));
  asciiToVK_.insert(KeyMapPair("Alt", fabgl::VK_LALT));
  asciiToVK_.insert(KeyMapPair("Control", fabgl::VK_LCTRL));
  asciiToVK_.insert(KeyMapPair("Shift", fabgl::VK_LSHIFT));
}

void TrsVirtualInterface::setScreenBuffer(uint8_t* buffer) {
  screenBuffer_ = buffer;
}

void TrsVirtualInterface::setScreenSizeProvider(const ScreenSizeProvider& p) {
  screenSizeProvider_ = p;
}

void TrsVirtualInterface::setChannel(DataChannel channel) {
  dataChannel_ = channel;
  frontendConnected_ = true;
}

void TrsVirtualInterface::setChannelClosed() {
  // By default, do nothing.
  dataChannel_ = [](const char*, size_t len) {};
  frontendConnected_ = false;
}

void TrsVirtualInterface::onPrinterWrite(uint8_t data) {
  auto msgSize = 2;
  char* msg = static_cast<char*>(malloc(msgSize));
  msg[0] = static_cast<char>(MSG_PRINTER_WRITE);
  msg[1] = static_cast<char>(data);
  onSendData(msg, msgSize);
  vTaskDelay(5 / portTICK_PERIOD_MS);
  free(msg);
}

void TrsVirtualInterface::onPrinterNewLine() {
  auto msgSize = 1;
  char* msg = static_cast<char*>(malloc(msgSize));
  msg[0] = static_cast<char>(MSG_PRINTER_NEW_LINE);
  onSendData(msg, msgSize);
  free(msg);
}

uint8_t TrsVirtualInterface::printerRead() {
  return frontendConnected_ ? 0x30 : 0xff;
}

void TrsVirtualInterface::onViFrontendData(const std::string& msg) {
  // vikb: Virtual Interface KeyBoard message.
  if (msg.rfind("vikb?", 0) == 0) {
    std::string query(msg.substr(5));
    bool shift = query.find("|shift") != std::string::npos;
    bool down = query.find("|down") != std::string::npos;

    size_t delimiter_pos = query.find("|");
    if (delimiter_pos != std::string::npos) {
      auto keyStr = query.substr(0, delimiter_pos);
      onViKeyPress(keyStr, down, shift);
    }
  } else {
    ESP_LOGW(TAG, "Unknown incoming VI message: '%s'", msg.c_str());
  }
}

// private
void TrsVirtualInterface::onSendData(const char* msg, size_t msgSize) {
  if (xSemaphoreTake(dataChannelSem_, (TickType_t) 10) == pdTRUE) {
    dataChannel_(msg, msgSize);
    xSemaphoreGive(dataChannelSem_);
  } else {
    ESP_LOGW(TAG, "Cannot obtain lock on sending channel.");
  }
}

// private
void TrsVirtualInterface::onViKeyPress(const std::string& key, bool down, bool shift) {
  std::map<std::string, VirtualKey>::iterator it = asciiToVK_.find(key);
  if (keyboard_ != NULL && it != asciiToVK_.end()) {
    keyboard_->injectVirtualKey(it->second, down, true);
  } else {
    ESP_LOGW(TAG, "Cannot find VirtualKey with value %s", key.c_str());
  }
}

// private static
void TrsVirtualInterface::startUpdateLoop(void* param) {
  TrsVirtualInterface* vi = static_cast<TrsVirtualInterface*>(param);

  while (true) {
    uint8_t screenWidth;
    uint8_t screenHeight;
    std::tie(screenWidth, screenHeight) = vi->screenSizeProvider_();

    auto screenSize = screenWidth * screenHeight;
    auto msgSize = 3 + screenSize;
    char* msg = static_cast<char*>(malloc(msgSize));
    if (vi->screenBuffer_ != NULL) {
      msg[0] = static_cast<char>(MSG_SCREEN_UPDATE);
      msg[1] = static_cast<char>(screenWidth);
      msg[2] = static_cast<char>(screenHeight);
      memcpy(&msg[3], vi->screenBuffer_, screenSize);
      vi->onSendData(msg, msgSize);
    }
    vTaskDelay(DATA_UPDATE_DELAY_MICROS / portTICK_PERIOD_MS);
    free(msg);
  }
}

// static
TrsVirtualInterface* TrsVirtualInterface::instance_ = NULL;