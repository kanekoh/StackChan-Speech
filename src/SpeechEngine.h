#pragma once
#include <Arduino.h>
#include <M5Unified.h>
#ifndef AUDIO_OUTPUT_M5_SPEAKER_H
#define AUDIO_OUTPUT_M5_SPEAKER_H
#include "AudioOutputM5Speaker.h"
#endif

namespace SpeechEngine {
  bool initSpeechEngine(const String& apiKey, const String& speakerNo = "3", AudioOutputM5Speaker* audioOut = nullptr);
  void enqueueText(const String& text); // 入力キューに追加
  void processSpeechQueue(); // 毎ループで呼び出し
  bool isSpeaking();
  void setSpeakerNo(const String& no);
  void playback();
}