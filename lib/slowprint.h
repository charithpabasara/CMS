#ifndef slowPrint_h
#define slowPrint_h

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>

static void delay_ms(int ms){
  Sleep(ms);
}

#else
#include <unistd.h>

static void delay_ms(int ms){
  usleep(ms * 1000);
}

#endif

static void slowPrint(const char *text, int speed){
  for (int i=0; text[i] != '\0'; i++)
  {
    printf("%c", text[i]);
    fflush(stdout);
    delay_ms(speed);
  }
}

#endif