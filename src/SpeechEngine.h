#pragma once
#include <Arduino.h>

namespace SpeechEngine {
  bool initSpeechEngine(const String& apiKey, const String& speakerNo = "3");
  void enqueueText(const String& text); // 入力キューに追加
  void processSpeechQueue(); // 毎ループで呼び出し
  bool isSpeaking();
  void setSpeakerNo(const String& no);
  void playback();
}