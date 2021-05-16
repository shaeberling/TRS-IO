//
// Created by Sascha Häberling sascha.codes in 2021.
//

#ifndef __TRS_VIRTUAL_INTERFACE_H__
#define __TRS_VIRTUAL_INTERFACE_H__

#include "fabgl.h"
#include <functional>
#include <stdint.h>
#include <string.h>
#include <map>
#include <tuple>

namespace {
typedef std::function<void(const char*, size_t len)> DataChannel;
typedef std::function<std::tuple<uint8_t, uint8_t>()> ScreenSizeProvider;
}  // namespace

// Provides a virtual interface for human input and output, meaning both the
// screen as well as the Keyboard is mapped.
// This sends out the screen contents periodically, and forwards key events it
// receives from the user interface.
// The default implementation uses a web interface, but this can be used to
// hook up any kind of interface.
class TrsVirtualInterface {
 public:
  static TrsVirtualInterface* instance();
  explicit TrsVirtualInterface();

  // Initializes the interface. Should be called once on startup. The keyboard
  // given will be used to inject virtual key events.
  void init(fabgl::Keyboard* kb);
  // The screen buffer pointer which will be periodically copied and sent to the
  // user interface.
  void setScreenBuffer(uint8_t* buffer);
  // Registers a function that provides us with the latest screen size.
  void setScreenSizeProvider(const ScreenSizeProvider& provider);
  // Sets the outward data channel through which data is sent to the UI. This
  // is agnostic to the unterlying data channel type, and can e.g. be using a
  // websocket.
  void setChannel(DataChannel channel);
  // Called when the data channel was closed so the virtual interface stops
  // sending data.
  void setChannelClosed();
  // Send a single byte to the printer.
  void onPrinterWrite(uint8_t data);
  // Send a new-line to the printer.
  void onPrinterNewLine();
  // Reads from the printer interface. Can be used to determine if a printer is
  // connected.
  uint8_t printerRead();
  // Called when data from the frontend is being sent to us. This could e.g.
  // be keyboard strokes and other user interaction.
  void onViFrontendData(const std::string& msg);

 private:
  static TrsVirtualInterface* instance_;
  fabgl::Keyboard* keyboard_;
  DataChannel dataChannel_;
  bool frontendConnected_;
  uint8_t* screenBuffer_;
  ScreenSizeProvider screenSizeProvider_;
  std::map<std::string, VirtualKey> asciiToVK_;
  static void startUpdateLoop(void* param);
  void onViKeyPress(const std::string& key, bool down, bool shift);
};

static const VirtualKey allKeys[] = {
    fabgl::VK_NONE,
    fabgl::VK_SPACE,
    fabgl::VK_0,
    fabgl::VK_1,
    fabgl::VK_2,
    fabgl::VK_3,
    fabgl::VK_4,
    fabgl::VK_5,
    fabgl::VK_6,
    fabgl::VK_7,
    fabgl::VK_8,
    fabgl::VK_9,
    fabgl::VK_KP_0,
    fabgl::VK_KP_1,
    fabgl::VK_KP_2,
    fabgl::VK_KP_3,
    fabgl::VK_KP_4,
    fabgl::VK_KP_5,
    fabgl::VK_KP_6,
    fabgl::VK_KP_7,
    fabgl::VK_KP_8,
    fabgl::VK_KP_9,
    fabgl::VK_a,
    fabgl::VK_b,
    fabgl::VK_c,
    fabgl::VK_d,
    fabgl::VK_e,
    fabgl::VK_f,
    fabgl::VK_g,
    fabgl::VK_h,
    fabgl::VK_i,
    fabgl::VK_j,
    fabgl::VK_k,
    fabgl::VK_l,
    fabgl::VK_m,
    fabgl::VK_n,
    fabgl::VK_o,
    fabgl::VK_p,
    fabgl::VK_q,
    fabgl::VK_r,
    fabgl::VK_s,
    fabgl::VK_t,
    fabgl::VK_u,
    fabgl::VK_v,
    fabgl::VK_w,
    fabgl::VK_x,
    fabgl::VK_y,
    fabgl::VK_z,
    fabgl::VK_A,
    fabgl::VK_B,
    fabgl::VK_C,
    fabgl::VK_D,
    fabgl::VK_E,
    fabgl::VK_F,
    fabgl::VK_G,
    fabgl::VK_H,
    fabgl::VK_I,
    fabgl::VK_J,
    fabgl::VK_K,
    fabgl::VK_L,
    fabgl::VK_M,
    fabgl::VK_N,
    fabgl::VK_O,
    fabgl::VK_P,
    fabgl::VK_Q,
    fabgl::VK_R,
    fabgl::VK_S,
    fabgl::VK_T,
    fabgl::VK_U,
    fabgl::VK_V,
    fabgl::VK_W,
    fabgl::VK_X,
    fabgl::VK_Y,
    fabgl::VK_Z,
    fabgl::VK_GRAVEACCENT,
    fabgl::VK_ACUTEACCENT,
    fabgl::VK_QUOTE,
    fabgl::VK_QUOTEDBL,
    fabgl::VK_EQUALS,
    fabgl::VK_MINUS,
    fabgl::VK_KP_MINUS,
    fabgl::VK_PLUS,
    fabgl::VK_KP_PLUS,
    fabgl::VK_KP_MULTIPLY,
    fabgl::VK_ASTERISK,
    fabgl::VK_BACKSLASH,
    fabgl::VK_KP_DIVIDE,
    fabgl::VK_SLASH,
    fabgl::VK_KP_PERIOD,
    fabgl::VK_PERIOD,
    fabgl::VK_COLON,
    fabgl::VK_COMMA,
    fabgl::VK_SEMICOLON,
    fabgl::VK_AMPERSAND,
    fabgl::VK_VERTICALBAR,
    fabgl::VK_HASH,
    fabgl::VK_AT,
    fabgl::VK_CARET,
    fabgl::VK_DOLLAR,
    fabgl::VK_POUND,
    fabgl::VK_EURO,
    fabgl::VK_PERCENT,
    fabgl::VK_EXCLAIM,
    fabgl::VK_QUESTION,
    fabgl::VK_LEFTBRACE,
    fabgl::VK_RIGHTBRACE,
    fabgl::VK_LEFTBRACKET,
    fabgl::VK_RIGHTBRACKET,
    fabgl::VK_LEFTPAREN,
    fabgl::VK_RIGHTPAREN,
    fabgl::VK_LESS,
    fabgl::VK_GREATER,
    fabgl::VK_UNDERSCORE,
    fabgl::VK_DEGREE,
    fabgl::VK_SECTION,
    fabgl::VK_TILDE,
    fabgl::VK_NEGATION,
    fabgl::VK_LSHIFT,
    fabgl::VK_RSHIFT,
    fabgl::VK_LALT,
    fabgl::VK_RALT,
    fabgl::VK_LCTRL,
    fabgl::VK_RCTRL,
    fabgl::VK_LGUI,
    fabgl::VK_RGUI,
    fabgl::VK_ESCAPE,
    fabgl::VK_PRINTSCREEN,
    fabgl::VK_SYSREQ,
    fabgl::VK_INSERT,
    fabgl::VK_KP_INSERT,
    fabgl::VK_DELETE,
    fabgl::VK_KP_DELETE,
    fabgl::VK_BACKSPACE,
    fabgl::VK_HOME,
    fabgl::VK_KP_HOME,
    fabgl::VK_END,
    fabgl::VK_KP_END,
    fabgl::VK_PAUSE,
    fabgl::VK_BREAK,
    fabgl::VK_SCROLLLOCK,
    fabgl::VK_NUMLOCK,
    fabgl::VK_CAPSLOCK,
    fabgl::VK_TAB,
    fabgl::VK_RETURN,
    fabgl::VK_KP_ENTER,
    fabgl::VK_APPLICATION,
    fabgl::VK_PAGEUP,
    fabgl::VK_KP_PAGEUP,
    fabgl::VK_PAGEDOWN,
    fabgl::VK_KP_PAGEDOWN,
    fabgl::VK_UP,
    fabgl::VK_KP_UP,
    fabgl::VK_DOWN,
    fabgl::VK_KP_DOWN,
    fabgl::VK_LEFT,
    fabgl::VK_KP_LEFT,
    fabgl::VK_RIGHT,
    fabgl::VK_KP_RIGHT,
    fabgl::VK_KP_CENTER,
    fabgl::VK_F1,
    fabgl::VK_F2,
    fabgl::VK_F3,
    fabgl::VK_F4,
    fabgl::VK_F5,
    fabgl::VK_F6,
    fabgl::VK_F7,
    fabgl::VK_F8,
    fabgl::VK_F9,
    fabgl::VK_F10,
    fabgl::VK_F11,
    fabgl::VK_F12,
    fabgl::VK_GRAVE_a,
    fabgl::VK_GRAVE_e,
    fabgl::VK_ACUTE_e,
    fabgl::VK_GRAVE_i,
    fabgl::VK_GRAVE_o,
    fabgl::VK_GRAVE_u,
    fabgl::VK_CEDILLA_c,
    fabgl::VK_ESZETT,
    fabgl::VK_UMLAUT_u,
    fabgl::VK_UMLAUT_o,
    fabgl::VK_UMLAUT_a,
    fabgl::VK_CEDILLA_C,
    fabgl::VK_TILDE_n,
    fabgl::VK_TILDE_N,
    fabgl::VK_UPPER_a,
    fabgl::VK_ACUTE_a,
    fabgl::VK_ACUTE_i,
    fabgl::VK_ACUTE_o,
    fabgl::VK_ACUTE_u,
    fabgl::VK_UMLAUT_i,
    fabgl::VK_EXCLAIM_INV,
    fabgl::VK_QUESTION_INV,
    fabgl::VK_ACUTE_A,
    fabgl::VK_ACUTE_E,
    fabgl::VK_ACUTE_I,
    fabgl::VK_ACUTE_O,
    fabgl::VK_ACUTE_U,
    fabgl::VK_GRAVE_A,
    fabgl::VK_GRAVE_E,
    fabgl::VK_GRAVE_I,
    fabgl::VK_GRAVE_O,
    fabgl::VK_GRAVE_U,
    fabgl::VK_INTERPUNCT,
    fabgl::VK_DIAERESIS,
    fabgl::VK_UMLAUT_e,
    fabgl::VK_UMLAUT_A,
    fabgl::VK_UMLAUT_E,
    fabgl::VK_UMLAUT_I,
    fabgl::VK_UMLAUT_O,
    fabgl::VK_UMLAUT_U,
    fabgl::VK_CARET_a,
    fabgl::VK_CARET_e,
    fabgl::VK_CARET_i,
    fabgl::VK_CARET_o,
    fabgl::VK_CARET_u,
    fabgl::VK_CARET_A,
    fabgl::VK_CARET_E,
    fabgl::VK_CARET_I,
    fabgl::VK_CARET_O,
    fabgl::VK_CARET_U
  };

#endif  // __TRS_VIRTUAL_INTERFACE_H__