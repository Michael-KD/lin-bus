#include "lin_bus.h"

const int BAUD_RATE = 19200;


void setup() {
    LIN lin(&Serial1, BAUD_RATE);
}

void loop() {
    
}