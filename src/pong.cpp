// #include <SPI.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #include "lin.h"
// #include <Arduino.h>

// // IMPORTED FROM LIN
// const uint32_t BAUD_RATE = 19200;

// //pins
// const uint8_t LIN_RXD = 0;
// const uint8_t LIN_TXD = 1;
// const uint8_t LIN_MCP_RESET = 2;
// const uint8_t LIN_CS = 3;

// //ids 
// const uint8_t PUPPET1_ID = 0x3b; //completely arbitrary
// const uint8_t PUPPET2_ID = 0x3c; //completely arbitrary

// size_t dataLength = 1;

// LIN::Master* master;

// // PONG DEFINES
// #define UP_BUTTON 5
// #define DOWN_BUTTON 6

// #define UP2_BUTTON 38
// #define DOWN2_BUTTON 39

// const unsigned long PADDLE_RATE = 33;
// const unsigned long BALL_RATE = 16;
// const uint8_t PADDLE_HEIGHT = 24;

// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels

// // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// #define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// void drawCourt() {display.drawRect(0, 0, 128, 64, WHITE);}

// uint8_t ball_x = 64, ball_y = 32;
// uint8_t ball_dir_x = 1, ball_dir_y = 1;
// unsigned long ball_update;

// unsigned long paddle_update;
// const uint8_t CPU_X = 12;
// uint8_t cpu_y = 16;

// const uint8_t PLAYER_X = 115;
// uint8_t player_y = 16;

// static bool up_state = false;
// static bool down_state = false;

// static bool up2_state = false;
// static bool down2_state = false;

// void setup() {
//     pinMode(LIN_CS, OUTPUT);
//     digitalWrite(LIN_CS, HIGH);

    master = new LIN::Master(BAUD_RATE, dataLength, 500000);
    master->startSerial(&Serial1);
    master->enable();


//     display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

//     display.display();
//     unsigned long start = millis();

//     display.clearDisplay();
//     drawCourt();

//     while(millis() - start < 2000);

//     display.display();

//     ball_update = millis();
//     paddle_update = ball_update;



// }

// void loop() {
//     bool update = false;
//     unsigned long time = millis();

//     // score vars
//     static uint8_t player_score = 0;
//     static uint8_t cpu_score = 0;

    
    // get button states from LIN
    uint8_t data[1] = {0};
    uint8_t player_1_move = master->requestData(data, PUPPET1_ID);
    if (player_1_move == 1) {
        if (data[0] == 1) {
            up_state = true;
            down_state = false;
        } else if (data[0] == 255) {
            down_state = true;
            up_state = false;
        }   
    } else {
        Serial.println("LIN timeout");
    }

    data[1] = {0};
    uint8_t player_2_move = master->requestData(data, PUPPET2_ID);
    if (player_2_move == 1) {
        if (data[0] == 1) {
            up2_state = true;
            down2_state = false;
        } else if (data[0] == 255) {
            down2_state = true;
            up2_state = false;
        }   
    } else {
        Serial.println("LIN timeout");
    }


//     // up_state |= (digitalRead(UP_BUTTON) == HIGH);
//     // down_state |= (digitalRead(DOWN_BUTTON) == HIGH);    

//     // up2_state |= (digitalRead(UP2_BUTTON) == HIGH);
//     // down2_state |= (digitalRead(DOWN2_BUTTON) == HIGH);

//     if(time > ball_update) {
//         uint8_t new_x = ball_x + ball_dir_x;
//         uint8_t new_y = ball_y + ball_dir_y;

//         // Check if we hit the vertical walls
//         if(new_x == 0 || new_x == 127) {
//             ball_dir_x = -ball_dir_x;
//             new_x += ball_dir_x + ball_dir_x;
//         }

//         // Check if we hit the horizontal walls.
//         if(new_y == 0 || new_y == 63) {
//             ball_dir_y = -ball_dir_y;
//             new_y += ball_dir_y + ball_dir_y;
//         }

//         // Check if we hit the CPU paddle
//         if(new_x == CPU_X && new_y >= cpu_y && new_y <= cpu_y + PADDLE_HEIGHT) {
//             ball_dir_x = -ball_dir_x;
//             new_x += ball_dir_x + ball_dir_x;
//         }

//         // Check if we hit the player paddle
//         if(new_x == PLAYER_X
//            && new_y >= player_y
//            && new_y <= player_y + PADDLE_HEIGHT)
//         {
//             ball_dir_x = -ball_dir_x;
//             new_x += ball_dir_x + ball_dir_x;
//         }

//         switch (new_x) {
//         case 1:
//         case 126:
//             if (new_x == 1) {
//                 // draw player 1 wins
//                 player_score++;
//                 // send score to LIN

//                 if (player_score == 5) {
//                     display.clearDisplay();
//                     display.setTextSize(2);
//                     display.setTextColor(WHITE);
//                     display.setCursor(10, 20);
//                     display.println("P1 wins!");
//                     display.display();
//                     player_score = 0;
//                 }
//             } else {
//                 // draw CPU wins
//                 cpu_score++;
//                 // send score to LIN


//                 if (cpu_score == 5) {
//                     display.clearDisplay();
//                     display.setTextSize(2);
//                     display.setTextColor(WHITE);
//                     display.setCursor(10, 20);
//                     display.println("P2 wins!");   
//                     display.display();
//                     cpu_score = 0;
//                 }
//             }   
//             delay(1000);
//             // reset positions
//             new_x = 64;
//             new_y = 32;
//             ball_x = 64;
//             ball_y = 32;
//             cpu_y = 16;
//             player_y = 16;
//             ball_dir_x = random(2) == 0 ? -1 : 1;
//             ball_dir_y = random(2) == 0 ? -1 : 1;
//             display.clearDisplay();
//             drawCourt();
//             break;
        
//         default:
//             break;
//         }


//         display.drawPixel(ball_x, ball_y, BLACK);
//         display.drawPixel(new_x, new_y, WHITE);
//         ball_x = new_x;
//         ball_y = new_y;

//         ball_update += BALL_RATE;

//         update = true;
//     }

//     if(time > paddle_update) {
//         paddle_update += PADDLE_RATE;

//         // CPU paddle
//         display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, BLACK);
//         if(up2_state) {
//             cpu_y -= 1;
//         }
//         if(down2_state) {
//             cpu_y += 1;
//         }

//         if(cpu_y < 1) cpu_y = 1;
//         if(cpu_y + PADDLE_HEIGHT > 63) cpu_y = 63 - PADDLE_HEIGHT;
//         display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, WHITE);

//         // Player paddle
//         display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
//         if(up_state) {
//             player_y -= 1;
//         }
//         if(down_state) {
//             player_y += 1;
//         }

//         // reset after updating paddles
//         up_state = down_state = false;
//         up2_state = down2_state = false;

//         if(player_y < 1) player_y = 1;
//         if(player_y + PADDLE_HEIGHT > 63) player_y = 63 - PADDLE_HEIGHT;
//         display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, WHITE);

//         update = true;
//     }

//     if(update)
//         display.display();
// }