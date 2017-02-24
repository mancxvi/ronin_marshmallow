/* William Oberndorfer
 * Ronin Marshmallow
 *
 * A Samurai Kirby clone for Arduino.
 */

#include <U8g2lib.h>
#include <Wire.h>

#define PLAYER_1_BUTTON 2
#define PLAYER_2_BUTTON 3

#define PLAYER_1_LED 11
#define PLAYER_2_LED 12
#define GO_LED 13
#define WAIT_LED 10

#define TONE_PIN 5
#define NOTE_E7 2637

void player_1_interrupt();
void player_2_interrupt();
void print_to_lcd(char *message);

volatile enum states { STOP, GO, WIN, WAIT } game_state = STOP;
volatile enum win { NONE, PLAYER_1, PLAYER_2 } winner = NONE;

U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

int duration;
int current;
int previous;
int response;

bool message;

String result;
char buf[10];

void setup()
{
    pinMode(PLAYER_1_BUTTON, INPUT_PULLUP);
    pinMode(PLAYER_2_BUTTON, INPUT_PULLUP);
    pinMode(PLAYER_1_LED, OUTPUT);
    pinMode(PLAYER_2_LED, OUTPUT);
    pinMode(GO_LED, OUTPUT);
    pinMode(WAIT_LED, OUTPUT);
    digitalWrite(PLAYER_1_LED, LOW);
    digitalWrite(PLAYER_2_LED, LOW);
    digitalWrite(GO_LED, LOW);
    digitalWrite(WAIT_LED, LOW);
    attachInterrupt(digitalPinToInterrupt(PLAYER_1_BUTTON),
                    player_1_interrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(PLAYER_2_BUTTON),
                    player_2_interrupt, FALLING);
    previous = millis();
    duration = random(3000,5000);
    u8g2.begin();
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_pxplustandynewtv_t_all);
        u8g2.drawStr(0,24,"...");
    } while ( u8g2.nextPage() );
    message = false;
}

void loop()
{
    current = millis();

    switch (game_state) {
    case STOP:
        digitalWrite(GO_LED, LOW);
        digitalWrite(WAIT_LED, HIGH);
        digitalWrite(PLAYER_1_LED, LOW);
        digitalWrite(PLAYER_2_LED, LOW);
        if (message == false) {
            print_to_lcd("...");
            message = true;
        }
        if ((current - previous) > duration) {
            game_state = GO;
            previous = millis();
            tone(TONE_PIN, NOTE_E7, 50);
        }
        break;
    case GO:
        digitalWrite(WAIT_LED, LOW);
        digitalWrite(GO_LED, HIGH);
        break;
    case WIN:
        if (winner == PLAYER_1) {
            digitalWrite(PLAYER_1_LED, HIGH);
            digitalWrite(PLAYER_2_LED, LOW);
            print_to_lcd("Player 1 wins!");
            message = false;
        }
        else if (winner == PLAYER_2) {
            digitalWrite(PLAYER_2_LED, HIGH);
            digitalWrite(PLAYER_1_LED, LOW);
            print_to_lcd("Player 2 wins!");
            message = false;
        }
        result = String(response, DEC) + " ms";
        delay(1000);
        winner = NONE;
        duration = random(3000,5000);
        game_state = WAIT;
        break;
    case WAIT:
        result.toCharArray(buf, 10);
        if (response == 0)
            print_to_lcd("Too soon!");
        else
            print_to_lcd(buf);
        delay(5000);
        game_state = STOP;
        previous = millis();
        break;
    default:
        break;
    }
}

void player_1_interrupt()
{
    if (!digitalRead(PLAYER_1_BUTTON)) {
        if (game_state == GO) {
            response = millis() - previous;
            winner = PLAYER_1;
        }
        else if (game_state == STOP) {
            response = 0;
            winner = PLAYER_2;
        }
        game_state = WIN;
    }
}

void player_2_interrupt()
{
    if (!digitalRead(PLAYER_2_BUTTON)) {
        if (game_state == GO) {
            response = millis() - previous;
            winner = PLAYER_2;
        }
        else if (game_state == STOP) {
            response = 0;
            winner = PLAYER_1;
        }
        game_state = WIN;
    }
}

void print_to_lcd(char *message)
{
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_pxplustandynewtv_t_all);
        u8g2.drawStr(0,24,message);
    } while ( u8g2.nextPage() );
}