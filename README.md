# SpeechEngine Library for M5Stack CoreS3 + VOICEVOX

`SpeechEngine` is a simple Arduino-compatible library for enabling VOICEVOX-based text-to-speech (TTS) on M5Stack CoreS3 devices. It handles VOICEVOX API communication, audio streaming over HTTPS, and MP3 playback via the onboard speaker.

## ✨ Features

- Easy-to-use `speakText("こんにちは")` API
- VOICEVOX API integration via `https://api.tts.quest`
- Streaming MP3 playback from secure (HTTPS) URLs
- Compatible with M5Stack CoreS3 and M5Unified
- Modular, extensible, and lightweight

## 📦 Installation

1. Clone this repository into your `lib/` folder in a PlatformIO project:

```bash
git clone https://github.com/kanekoh/ScackChan-Speech.git
```

2. Or install it as a PlatformIO library (coming soon).

## 📚 Dependencies

In your `platformio.ini`, include:

```ini
lib_deps =
  M5Stack/M5Unified@^0.1.9
  earlephilhower/Audio@^1.0.4
  bblanchon/ArduinoJson@^6.21.2
```

Also ensure your board supports HTTPS (`WiFiClientSecure`).

## 🛠 Usage Example

```cpp
#include <M5Unified.h>
#include "SpeechEngine.h"

void setup() {
  M5.begin();
  initSpeechEngine("your_voicevox_api_key", "3");
  speakText("こんにちは、スタックチャンです！");
}

void loop() {
  // Wait while speaking
  if (!isSpeaking()) {
    delay(2000);
    speakText("またお話しましょう。");
  }
}
```

## 🛡 License

This library incorporates code from:

- **ESP8266Audio** by Earle F. Philhower, III (GPLv3)
- Modified by robo8080 in the AI_StackChan2 project

Your modifications must preserve the GPLv3 license.

## 🤝 Credits

- [Earle F. Philhower, III](https://github.com/earlephilhower) - Original streaming audio code
- [robo8080](https://github.com/robo8080) - Adaptation for M5Stack & VOICEVOX
- [YourNameHere] - This library's integration & packaging

---

Enjoy talking to your Stack-chan!
