/* William Oberndorfer
 * Ronin Marshmallow
 *
 * A Samurai Kirby clone for Arduino.
 */

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

volatile enum states { STOP, GO, WIN, WAIT } game_state = STOP;
volatile enum win { NONE, PLAYER_1, PLAYER_2 } winner = NONE;

int duration;
int current;
int previous;

void setup()
{
    pinMode(PLAYER_1_BUTTON, INPUT);
    pinMode(PLAYER_2_BUTTON, INPUT);
    pinMode(PLAYER_1_LED, OUTPUT);
    pinMode(PLAYER_2_LED, OUTPUT);
    pinMode(GO_LED, OUTPUT);
    pinMode(WAIT_LED, OUTPUT);
    digitalWrite(PLAYER_1_LED, LOW);
    digitalWrite(PLAYER_2_LED, LOW);
    digitalWrite(GO_LED, LOW);
    digitalWrite(WAIT_LED, LOW);
    attachInterrupt(digitalPinToInterrupt(PLAYER_1_BUTTON),
                    player_1_interrupt, RISING);
    attachInterrupt(digitalPinToInterrupt(PLAYER_2_BUTTON),
                    player_2_interrupt, RISING);
    previous = millis();
    duration = random(3000,5000);
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
        if ((current - previous) > duration) {
            game_state = GO;
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
        }
        else if (winner == PLAYER_2) {
            digitalWrite(PLAYER_2_LED, HIGH);
            digitalWrite(PLAYER_1_LED, LOW);
        }
        delay(1000);
        winner = NONE;
        previous = millis();
        duration = random(3000,5000);
        game_state = WAIT;
        break;
    case WAIT:
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
    if (digitalRead(PLAYER_1_BUTTON)) {
        if (game_state == GO) {
            winner = PLAYER_1;
        }
        else {
            winner = PLAYER_2;
        }
        game_state = WIN;
    }
}

void player_2_interrupt()
{
    if (digitalRead(PLAYER_2_BUTTON)) {
        if (game_state == GO) {
            winner = PLAYER_2;
        }
        else {
            winner = PLAYER_1;
        }
        game_state = WIN;
    }
}