#pragma once
#include <Arduino.h>

bool initVoicevoxClient();
String fetchVoicevoxUrl(const String& apiKey, const String& text, const String& speakerParam);
void playMP3FromUrl(const String& url);
bool isMP3Running();
void updatePlayback();
