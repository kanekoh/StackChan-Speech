#include <M5Unified.h>
#include "VoicevoxClient.h"
#include <AudioGeneratorMP3.h>
#include <AudioFileSourceBuffer.h>
#include "AudioFileSourceHTTPSStream.h"
#ifndef AUDIO_OUTPUT_M5_SPEAKER_H
#define AUDIO_OUTPUT_M5_SPEAKER_H
#include "AudioOutputM5Speaker.h"
#endif
#include "rootCA.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "I2SBlockingGuard.h"
#include "PlaybackQueue.h"

static AudioGeneratorMP3* mp3 = nullptr;
static AudioFileSourceBuffer* buff = nullptr;
static AudioFileSourceHTTPSStream* file = nullptr;
static uint8_t* preallocateBuffer = nullptr;
static const int bufferSize = 30 * 1024;
static bool playbackActive = false;
static AudioOutputM5Speaker* audioOut = nullptr;

bool initVoicevoxClient() {
  preallocateBuffer = (uint8_t*)malloc(bufferSize);
  if (!preallocateBuffer) {
    Serial.println("[VoicevoxClient] ❌ Failed to allocate buffer.");
    return false;
  }
  mp3 = new AudioGeneratorMP3();
  return true;
}

bool isMP3Running() {
  return playbackActive;
}

void setAudioOutput(AudioOutputM5Speaker* out) {
  audioOut = out;
}

AudioOutputM5Speaker* getAudioOutputM5Speaker() {
  return audioOut;
}

static String urlencode(const String& msg) {
  String encoded = "";
  char hex[4];
  for (int i = 0; i < msg.length(); i++) {
    char c = msg[i];
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += c;
    } else {
      sprintf(hex, "%%%02X", (unsigned char)c);
      encoded += hex;
    }
  }
  return encoded;
}

static String voicevox_tts_url(const char* url, const char* root_ca) {
  Serial.println("[voicevox_tts_url] Start");

  String mp3url = "";
  WiFiClientSecure client;
  client.setCACert(root_ca);
  HTTPClient https;

  if (https.begin(client, url)) {
    Serial.println("[voicevox_tts_url] HTTPS begin succeeded");

    int httpCode = https.GET();
    Serial.printf("[voicevox_tts_url] HTTP GET code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = https.getString();
      Serial.println("[voicevox_tts_url] Payload received:");
      Serial.println(payload);

      DynamicJsonDocument doc(1024);
      auto err = deserializeJson(doc, payload);
      if (err == DeserializationError::Ok) {
        Serial.println("[voicevox_tts_url] JSON deserialization OK");

        if (doc["success"] && doc["mp3StreamingUrl"]) {
          mp3url = doc["mp3StreamingUrl"].as<String>();
          Serial.println("[voicevox_tts_url] mp3StreamingUrl = " + mp3url);
        } else {
          Serial.println("[voicevox_tts_url] Missing or invalid fields in JSON");
        }
      } else {
        Serial.print("[voicevox_tts_url] JSON deserialization failed: ");
        Serial.println(err.c_str());
      }
    } else {
      Serial.printf("[voicevox_tts_url] HTTP GET failed, code: %d\n", httpCode);
    }
    https.end();
  } else {
    Serial.println("[voicevox_tts_url] HTTPS begin failed");
  }

  Serial.println("[voicevox_tts_url] End");
  return mp3url;
}

String fetchVoicevoxUrl(const String& apiKey, const String& text, const String& speakerParam) {
  String url = "https://api.tts.quest/v3/voicevox/synthesis?key=" + apiKey + "&text=" + urlencode(text) + speakerParam;
  return voicevox_tts_url(url.c_str(), root_ca);
}

void playMP3FromUrlBlocking(const String& mp3url) {
  Serial.println("[VoicevoxClient] Starting playback...");
  Serial.println("[VoicevoxClient] MP3 URL: " + mp3url);

  I2SBlockingGuard guard(I2SMode::Playing);  // 🔒 ロックをスコープに閉じる

  file = new AudioFileSourceHTTPSStream(mp3url.c_str(), root_ca);
  if (!file) {
    Serial.println("[VoicevoxClient] Failed to create HTTPS stream.");
    return;
  }

  buff = new AudioFileSourceBuffer(file, preallocateBuffer, bufferSize);
  if (!buff) {
    Serial.println("[VoicevoxClient] Failed to create audio buffer.");
    return;
  }

  if (audioOut == nullptr) {
    Serial.println("[VoicevoxClient] Audio output not set, using default M5 Speaker.");
    delete buff; delete file;
    return;
  }
  bool success = mp3->begin(buff, audioOut);
  if (!success) {
    playbackActive = false;
    Serial.println("[VoicevoxClient] MP3 playback failed to start.");
  } else {
    playbackActive = true;
    Serial.println("[VoicevoxClient] MP3 playback started.");
  }

  while (mp3->loop()) {
    delay(1);
  }

  mp3->stop();

  if (file) {  
    file->close();
    delete file; 
    file = nullptr; 
  }

  if (buff) { delete buff; buff = nullptr; }

  Serial.println("[VoicevoxClient] MP3 playback finished.");
  delay(200);
  playbackActive = false;

}
