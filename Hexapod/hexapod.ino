#include <DynamixelSDK.h>
#include "hexa_config.h"

// Control table address
#define ADDR_PRO_TORQUE_ENABLE 24 // Control table address is different in Dynamixel model
#define ADDR_PRO_GOAL_POSITION 30
#define ADDR_PRO_PRESENT_POSITION 36

// Protocol version
#define PROTOCOL_VERSION 1.0 // See which protocol version is used in the Dynamixel

#define BAUDRATE 1000000
#define DEVICENAME "OpenCR_DXL_Port" // This definition only has a symbolic meaning and does not affect to any functionality

#define TORQUE_ENABLE 1  // Value for enabling the torque
#define TORQUE_DISABLE 0 // Value for disabling the torque

#define DXL_MOVING_STATUS_THRESHOLD 20 // Dynamixel moving status threshold

#define ESC_ASCII_VALUE 0x1b

#define CMD_SERIAL Serial

int getch()
{
    while (1)
    {
        if (CMD_SERIAL.available() > 0)
        {
            break;
        }
    }

    return CMD_SERIAL.read();
}

int kbhit(void)
{
    return CMD_SERIAL.available();
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    Serial.println("Start..");

    // Initialize PortHandler instance
    // Set the port path
    // Get methods and members of PortHandlerLinux or PortHandlerWindows
    dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);

    // Initialize PacketHandler instance
    // Set the protocol version
    // Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
    dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

    // Open port
    if (portHandler->openPort())
    {
        Serial.print("Succeeded to open the port!\n");
    }
    else
    {
        Serial.print("Failed to open the port!\n");
        return;
    }

    // Set port baudrate
    if (portHandler->setBaudRate(BAUDRATE))
    {
        Serial.print("Succeeded to change the baudrate!\n");
    }
    else
    {
        Serial.print("Failed to change the baudrate!\n");
        return;
    }
}

void loop(void)
{
    Serial.print("Press any key to continue! (or press q to quit!)\n");
    if (getch() == 'q')
        break;
}