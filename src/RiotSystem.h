#ifndef RIOTSYSTEM_H_
#define RIOTSYSTEM_H_

#include <Arduino.h>

#define INTERRUPT_PIN 0 //D3
#define INTERRUPT_SECONDS 0 // Wait for INTERRUPT_SECONDS to trigger // BUGGY

enum SYSTEM_STATUS {
    SYS_NORMAL,
    SYS_BACKUP,
};

extern SYSTEM_STATUS SYSTEM;

extern boolean startTimer;
extern long now;
extern long lastTrigger;

void backUpRead();

#endif
