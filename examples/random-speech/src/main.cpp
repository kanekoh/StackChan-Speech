#include <M5Unified.h>
#include <Avatar.h>
#include "WiFiHelper.h" 
#include "SpeechEngine.h"  // 追加
#include "SDUtils.h"

using namespace m5avatar;

Avatar avatar;

void outputMessage(String message) {
  Serial.println(message);
  M5.Display.println(message);
}

void speechTask(void*) {
  for (;;) {
    SpeechEngine::processSpeechQueue();
    delay(100);
  }
}

void playbackTask(void*) {
  for (;;) {
    SpeechEngine::playback();
    delay(5);
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);  // ★これを追加
  delay(100);            // ★シリアル同期用の短い待機
  std::vector<String> keys;

  outputMessage("Initilize SD card");
  if (initSDCard()) {
    if (readLinesFromSD("/apikey.txt", keys) && keys.size() >= 3) {
      String voicevoxKey = keys[1];
      SpeechEngine::initSpeechEngine(voicevoxKey, "3");  // APIキーだけ渡す
      outputMessage("Scceeded to read /apikey.txt");
    } else {
      M5.Lcd.println("APIキー読み込み失敗");
    }
  } else {
    outputMessage("Initilize SD card failed.");
  }

  outputMessage("Trying to connect to Wi-Fi...");
  bool success = WiFiHelper::setupWiFi();  
  if (success) {
    String ip = WiFi.localIP().toString();
    outputMessage("Connected! IP: " + ip);
  } else {
    outputMessage("Wi-Fi connection failed. Check settings.");
  }

  // スピーカーの設定
  auto spk_cfg = M5.Speaker.config();
  spk_cfg.sample_rate = 96000;
  spk_cfg.task_pinned_core = APP_CPU_NUM;
  M5.Speaker.config(spk_cfg);
  M5.Speaker.begin();
  M5.Speaker.setVolume(100);

  delay(1000);
  avatar.init();  
  avatar.addTask(speechTask, "speech", 6144);
  avatar.addTask(playbackTask, "playback", 6144);
  avatar.setSpeechFont(&fonts::efontJA_16);
  delay(1000);

  // 最初のあいさつ
  SpeechEngine::enqueueText("やっほー！スタックチャンだよ。今日はしゃべり方のテストをするね！");
}

unsigned long lastSpeak = 0;
unsigned long interval = 70000; // 約70秒間隔で発話
int mode = 0; // 0 = 旧方式、1 = キュー方式
bool pending = false;

void loop() {
  M5.update();

  unsigned long now = millis();
  if (!pending && now - lastSpeak > interval) {
    lastSpeak = now;
    pending = true;

    if (mode == 0) {
      avatar.setSpeechText("昔のしゃべり方");
      delay(500);
      // 旧方式：話し終わったら次のセリフを送る
      SpeechEngine::enqueueText("これは昔ながらのしゃべり方だよ。ひとこと話し終わるまで、次のことは考えないの。");
    } else {
      avatar.setSpeechText("ちょっと賢いよ。");
      delay(500);
      // キュー方式：まとめて送ることでスムーズに話せる
      SpeechEngine::enqueueText("次のしゃべり方は、ちょっと賢いよ。");
      SpeechEngine::enqueueText("実は、あらかじめ何を言うか、ぜんぶ考えておいて、ぜんぶまとめてお願いしてあるの。");
      SpeechEngine::enqueueText("だから、前のセリフが終わるのを待たずに、次の音声の準備ができるんだよ！");
    }
  }

  // 話し終わったらモード切り替え
  if (pending && !SpeechEngine::isSpeaking()) {

    pending = false;
    mode = (mode + 1) % 2;

    if (mode == 0) {
      interval = 75000; // ゆったり
    } else {
      interval = 65000; // 少し短く
    }
  }

  delay(1000);
}