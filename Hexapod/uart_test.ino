void setup()
{
    Serial.begin(115200);
    Serial.println("Receive");
}

void loop()
{
    while (Serial.available())
    {
        char data = Serial.read();
        Serial.print(data);
    }
}