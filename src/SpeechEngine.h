#pragma once
#include <Arduino.h>
#include <M5Unified.h>
#ifndef AUDIO_OUTPUT_M5_SPEAKER_H
#define AUDIO_OUTPUT_M5_SPEAKER_H
#include "AudioOutputM5Speaker.h"
#endif


// voice vox 
enum class SpeechTone {
  Happy = 1,
  Angry = 7,
  Sad = 76,
  Doubt = 75,
  Sleepy = 22,
  Neutral = 3,
};

struct Speech {
  String speakerNo;
  String text;
};

String toneToString(SpeechTone);

namespace SpeechEngine {
  bool initSpeechEngine(const String& apiKey, const String& speakerNo = "3", AudioOutputM5Speaker* audioOut = nullptr);
  void enqueueText(const String& text); // 入力キューに追加
  void enqueueText(SpeechTone, const String& text);
  void processSpeechQueue(); // 毎ループで呼び出し
  bool isSpeaking();
  void setSpeakerNo(const String& no);
  void playback();
  AudioOutputM5Speaker* getAudioOutput();

}