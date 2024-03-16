// C++ code
//
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(32, 16, 2);

const int buttonPin1 = 5; // Pin for button 1
const int buttonPin2 = 6; // Pin for button 2
const int buttonPin3 = 7; // Pin for button 3
unsigned long buttonPressStartTime = 0;

int modejam;
int modesetting;
int stopwatchstart;
int enablealarm;

volatile int seconds1 = 0;
volatile int seconds2 = 0;
volatile int minutes1 = 0;
volatile int minutes2 = 0;
volatile int hours1 = 0;
volatile int hours2 = 0;
volatile bool timeChanged = false;

unsigned long startTime = 0;
unsigned long elapsedTime = 0;

volatile uint8_t S_seconds = 0;
volatile uint8_t S_minutes = 0;
volatile uint8_t S_hours = 0;

uint8_t alarmHours1 = 0;
uint8_t alarmHours2 = 0;
uint8_t alarmMinutes1 = 0;
uint8_t alarmMinutes2 = 0;

int posisiDigitJam;
int posisiDigitAlarm;

void setup()
{
    Serial.begin(9600);

    cli();      // Disable interrupts
    TCCR1A = 0; // Clear Timer1 control registers
    TCCR1B = 0;
    TCNT1 = 0;                           // Initialize counter value to 0
    OCR1A = 15624;                       // Set the compare match value (1 Hz)
    TCCR1B |= (1 << WGM12);              // Configure timer for CTC mode
    TCCR1B |= (1 << CS12) | (1 << CS10); // Set prescaler to 1024
    TIMSK1 |= (1 << OCIE1A);             // Enable Timer1 compare match interrupt
    sei();                               // Enable interrupts

    pinMode(buttonPin1, INPUT); // Set button 1 as input
    pinMode(buttonPin2, INPUT); // Set button 2 as input
    pinMode(buttonPin3, INPUT); // Set button 3 as input

    lcd.init();
    // lcd.begin(16, 2);
    // lcd.setBacklight(255);

    lcd.setCursor(0, 0);
    lcd.backlight();
    lcd.display();
    lcd.print("Digital Clock"); // pada baris pertama dituliskan nyebarilmu.com

    lcd.setCursor(00, 1);
    lcd.print("Arif & Rafael"); // pada baris kedua dituliskan LCD + DS3231
    delay(5000);                // Waktu jeda
    lcd.clear();
}

void loop()
{
    if (digitalRead(buttonPin1) == HIGH)
    { // If button 1 is not pressed
        lcd.clear();
        buttonPressStartTime = millis();
        while (digitalRead(buttonPin1) == HIGH)
        {
            delay(10); // Small delay to improve responsiveness

            // Check if the button is released after a short delay (e.g., 500 ms)
            if (millis() - buttonPressStartTime >= 50 && digitalRead(buttonPin1) == LOW)
            {
                if (modesetting == 0)
                {
                    modejam = ((modejam + 1) % 3);
                    if (modejam == 0)
                    {
                        Serial.println("mode jam");
                    }
                    if (modejam == 1)
                    {
                        Serial.println("mode alarm");
                    }
                    if (modejam == 2)
                    {
                        Serial.println("mode stopwatch");
                    }
                }

                if (modesetting == 1)
                {
                    if (modejam == 0)
                    {
                        Serial.println("Geser digit Jam");
                        posisiDigitJam++;
                        if (posisiDigitJam > 5)
                        {
                            posisiDigitJam = 0;
                        }
                    }
                    if (modejam == 1)
                    {
                        Serial.println("Geser digit alarm");
                    }
                }

                delay(500); // debounce delay
                break;      // Exit the loop after printing "world"
            }

            // Check if the button is held for 3 seconds
            if (millis() - buttonPressStartTime >= 3000)
            {
                if (modejam == 0)
                {
                    if ((modesetting == 0))
                    {
                        modesetting = 1;
                        Serial.println("Mode Setting Jam");
                    }
                    else
                    {
                        modesetting = 0;
                        Serial.println("Keluar Mode Setting Jam");
                    }
                }
                else if (modejam == 1)
                {
                    if ((modesetting == 0))
                    {
                        modesetting = 1;
                        Serial.println("Mode Setting Alarm");
                    }
                    else
                    {
                        modesetting = 0;
                        Serial.println("Keluar Mode Setting Alarm");
                    }
                }

                while (digitalRead(buttonPin1) == HIGH)
                {
                }      // Wait for the button to be released
                break; // Exit the loop once "hello world" is printed
            }
        }
    }

    if (digitalRead(buttonPin2) == HIGH)
    { // If button 2 is not pressed
        lcd.clear();
        // Serial.println("mode");
        delay(500); // debounce delay
        if (modejam == 0 && modesetting == 1)
        {
            Serial.println("Tambah Digit Jam");
            increaseDigitJam();
        }

        if (modejam == 1 && modesetting == 1)
        {
            Serial.println("Tambah Digit Alarm");
        }

        if (modejam == 1 && modesetting == 0)
        {
            if (enablealarm == 0)
            {
                Serial.println("Alarm Enable");
                enablealarm = 1;
            }
            else if (enablealarm == 1)
            {
                Serial.println("Alarm Disable");
                enablealarm = 0;
            }
        }

        if (modejam == 2)
        {
            if (stopwatchstart == 0)
            {
                Serial.println("Mulai stopwatch");
                stopwatchstart = 1;
            }
            else if (stopwatchstart == 1)
            {
                Serial.println("Stopwatch stop");
                stopwatchstart = 0;
            }
        }

        while (digitalRead(buttonPin2) == HIGH)
        {
        } // Wait for the button to be released
    }

    if (digitalRead(buttonPin3) == HIGH)
    { // If button 3 is not pressed
        lcd.clear();
        delay(500);
        if (modejam == 0 && modesetting == 1)
        {
            Serial.println("Kurang Digit Jam");
            decreaseDigitJam();
        }

        if (modejam == 1 && modesetting == 1)
        {
            Serial.println("Kurang Digit Alarm");
        }

        if (modejam == 2 && stopwatchstart == 0)
        {
            Serial.println("Reset stopwatch");
            S_seconds = 0;
            S_minutes = 0;
            S_hours = 0;
        }
    }

    if (modejam == 0)
    {
        // tampilan jam digital
        lcd.setCursor(1, 0);
        lcd.print("Clock Mode");
        if (timeChanged)
        {
            lcd.setCursor(1, 1);
            lcd.print("Time: ");
            lcd.print(hours2);
            lcd.print(hours1);
            lcd.print(":");
            lcd.print(minutes2);
            lcd.print(minutes1);
            lcd.print(":");
            lcd.print(seconds2);
            lcd.print(seconds1);
            timeChanged = false;
        }
    }
    else if (modejam == 1)
    {
        // tampilan alarm
        lcd.setCursor(1, 0);
        lcd.print("Alarm Mode");
        if (enablealarm == 1)
        {
            lcd.setCursor(15, 0);
            lcd.print("E");
        }
        else if (enablealarm == 0)
        {
            lcd.setCursor(15, 0);
            lcd.print("D");
        }
    }
    else if (modejam == 2)
    {
        // tampilan stopwatch
        lcd.setCursor(1, 0);
        lcd.print("StopWatch Mode");
        if (timeChanged)
        {
            lcd.setCursor(1, 1);
            lcd.print("Time: ");
            if (S_hours < 10)
                lcd.print("0");
            lcd.print(S_hours);
            lcd.print(":");
            if (S_minutes < 10)
                lcd.print("0");
            lcd.print(S_minutes);
            lcd.print(":");
            if (S_seconds < 10)
                lcd.print("0");
            lcd.println(S_seconds);
            timeChanged = false;
        }
    }
}

void increaseDigitJam()
{
    // Fungsi untuk menambah digit
    // Dalam kasus ini, misalnya, menambah detik alarm
    if (posisiDigitJam == 0)
    {
        seconds1++;
        if (seconds1 > 9)
        {
            seconds1 = 0;
        }
    }
    else if (posisiDigitJam == 1)
    {
        seconds2++;
        if (seconds2 > 5)
        {
            seconds2 = 0;
        }
    }
    else if (posisiDigitJam == 2)
    {
        minutes1++;
        if (minutes1 > 9)
        {
            minutes1 = 0;
        }
    }
    else if (posisiDigitJam == 3)
    {
        minutes2++;
        if (minutes2 > 5)
        {
            minutes2 = 0;
        }
    }
    else if (posisiDigitJam == 4)
    {
        hours1++;
        if (hours2 < 2)
        {
            if (hours1 > 9)
            {
                hours1 = 0;
            }
        }
        else
        {
            if (hours1 > 4)
            {
                hours1 = 0;
            }
        }
    }
    else if (posisiDigitJam == 5)
    {
        hours2++;
        if (hours2 > 2)
        {
            hours2 = 0;
        }
    }
}

void decreaseDigitJam()
{
    // Fungsi untuk mengurangi digit
    // Dalam kasus ini, misalnya, mengurangi detik alarm
    if (posisiDigitJam == 0)
    {
        seconds1--;
        if (seconds1 < 0)
        {
            seconds1 = 9;
        }
    }
    else if (posisiDigitJam == 1)
    {
        seconds2--;
        if (seconds2 < 0)
        {
            seconds2 = 5;
        }
    }
    else if (posisiDigitJam == 2)
    {
        minutes1--;
        if (minutes1 < 0)
        {
            minutes1 = 9;
        }
    }
    else if (posisiDigitJam == 3)
    {
        minutes2--;
        if (minutes2 < 0)
        {
            minutes2 = 5;
        }
    }
    else if (posisiDigitJam == 4)
    {
        hours1--;
        if (hours2 < 2)
        {
            if (hours1 < 0)
            {
                hours1 = 9;
            }
        }
        else
        {
            if (hours1 < 0)
            {
                hours1 = 4;
            }
        }
    }
    else if (posisiDigitJam == 5)
    {
        hours2--;
        if (hours2 < 0)
        {
            hours2 = 2;
        }
    }
}

ISR(TIMER1_COMPA_vect)
{
    seconds1++;

    if (seconds1 > 9)
    {
        seconds1 = 0;
        seconds2++;
        if (seconds2 > 5)
        {
            minutes1++;
            seconds1 = 0;
            seconds2 = 0;
            if (minutes1 > 9)
            {
                minutes2++;
                minutes1 = 0;
                seconds1 = 0;
                seconds2 = 0;
                if (minutes2 > 5)
                {
                    hours1++;
                    minutes2 = 0;
                    minutes1 = 0;
                    seconds1 = 0;
                    seconds2 = 0;
                    if (hours2 < 2)
                    {
                        if (hours1 > 9)
                        {
                            hours2++;
                            hours1 = 0;
                            minutes2 = 0;
                            minutes1 = 0;
                            seconds1 = 0;
                            seconds2 = 0;
                        }
                    }
                    else if (hours2 == 2)
                    {
                        if (hours1 > 4)
                        {
                            hours2 = 0;
                            hours1 = 0;
                            minutes2 = 0;
                            minutes1 = 0;
                            seconds1 = 0;
                            seconds2 = 0;
                        }
                    }
                }
            }
        }
    }

    timeChanged = true; // Set the flag to signal time change

    if (stopwatchstart)
    {
        S_seconds++;
        if (S_seconds == 60)
        {
            S_seconds = 0;
            S_minutes++;
            if (S_minutes == 60)
            {
                S_minutes = 0;
                S_hours++;
                if (S_hours == 24)
                {
                    S_hours = 0;
                }
            }
        }
    }

    // Update alarm seconds here
    alarmSeconds = 0;

    input = '0';
}