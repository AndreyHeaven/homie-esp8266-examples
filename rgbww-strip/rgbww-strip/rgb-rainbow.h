// #if !defined _COMMS_H
// #define _COMMS_H

//extern int commsEnabled; // global variable. definition only. no assignment
                         // see note below about the "extern"

void rainbow_setup();
void rainbow_loop();
void setColorRainbow (unsigned char red, unsigned char green, unsigned char blue);
long HSBtoRGB(float _hue, float _sat, float _brightness);

// #endif
