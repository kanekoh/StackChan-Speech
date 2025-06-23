#include "SpeechEngine.h"
#include "VoicevoxClient.h"
#include <M5Unified.h>
#include <queue>
#include "PlaybackQueue.h"
#ifndef AUDIO_OUTPUT_M5_SPEAKER_H
#define AUDIO_OUTPUT_M5_SPEAKER_H
#include "AudioOutputM5Speaker.h"
#endif

String toneToString(SpeechTone tone){
  switch(tone) {
    case SpeechTone::Happy: return "1";
    case SpeechTone::Angry: return "7";
    case SpeechTone::Doubt: return "75";
    case SpeechTone::Neutral: return "3";
    case SpeechTone::Sad: return "76";
    case SpeechTone::Sleepy: return "22";
  }
}

namespace SpeechEngine {
  static bool initialized = false;

  static String _apiKey;
  static String _speakerNo;
  static std::queue<Speech> speechQueue;
  static String currentUrl = "";

  bool initSpeechEngine(const String& apiKey, const String& speakerNo, AudioOutputM5Speaker* audioOut) {
    _apiKey = apiKey;
    _speakerNo = speakerNo;
    setAudioOutput(audioOut);
    if (initVoicevoxClient()) {
      initialized = true;
      return true;
    } else {
      return false;
    }
  }

  bool isInitialized() {
    return initialized;
  }

  void enqueueText(const String& text) {
    Speech speech{_speakerNo, text};
    speechQueue.push(speech);
    Serial.println("Enqueued text: " + text);
  }

  void enqueueText(SpeechTone tone, const String& text) {
    Speech speech{toneToString(tone), text};
    speechQueue.push(speech);
    Serial.println("Enqueued text: " + text + " with speaker no. " + toneToString(tone));
  }

  void processSpeechQueue() {
    if (!initialized) return;
    
    // 再生中でない & URLがあるなら再生開始
    if (!currentUrl.isEmpty() && !isMP3Running()) {
      Serial.println("[SpeechEngine] Starting playback from URL");
      M5.Mic.end();
      playbackQueue.enqueue(currentUrl);
      // playMP3FromUrl(currentUrl);
      currentUrl = "";
      return;
    }

    // MP3再生中でもURL未取得なら次の音声のURLを先に取得
    if (currentUrl.isEmpty() && !speechQueue.empty()) {
      Serial.println("Fetching next speech URL...");
      Speech speech = speechQueue.front();
      String param = "&speaker=" + speech.speakerNo;
      currentUrl = fetchVoicevoxUrl(_apiKey, speech.text, param);

      if (currentUrl.isEmpty()) {
        Serial.println("[SpeechEngine] Failed to fetch MP3 URL");
      } else {
        Serial.println("[SpeechEngine] MP3 URL fetched successfully: " + currentUrl);
      }

      speechQueue.pop();
    }

  }

  void playback() {
    updatePlayback();
  }

  bool isSpeaking() {
    return isMP3Running();
  }

  void setSpeakerNo(const String& no) {
    _speakerNo = no;
  }

  AudioOutputM5Speaker* getAudioOutput(){
    return getAudioOutputM5Speaker();
  }

}
