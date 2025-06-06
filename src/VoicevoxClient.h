#pragma once
#include <Arduino.h>
#include <M5Unified.h>
#ifndef AUDIO_OUTPUT_M5_SPEAKER_H
#define AUDIO_OUTPUT_M5_SPEAKER_H
#include "AudioOutputM5Speaker.h"
#endif

bool initVoicevoxClient();
String fetchVoicevoxUrl(const String& apiKey, const String& text, const String& speakerParam);
void playMP3FromUrl(const String& url);
bool isMP3Running();
void updatePlayback();
char* getAudioOutput();
void setAudioOutput(AudioOutputM5Speaker* out);
AudioOutputM5Speaker* getAudioOutputM5Speaker();