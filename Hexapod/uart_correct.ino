// Serial : Computer(Serial Monitor) - OpenCR
// Serial1 : OpenCR - STM32F107

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
}

void loop()
{
    while (Serial1.available())
    {
        char data = (char)Serial1.read();
        Serial.print(data);
    }
}
