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
    int dxl_comm_result = COMM_TX_FAIL;
    uint8_t dxl_error = 0;
    uint16_t dxl_present_position = 0;
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

    // Torque Enable
    for (int i = 1; i < TotalMotors + 1; i++)
    {
        dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, i, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }
        else
        {
            Serial.print("Dynamixel has been successfully connected \n");
        }
    }

    while (1)
    {
        Serial.print("Press any key to continue! (or press q to quit!)\n");
        if (getch() == 'q')
            break;

        // Init Start Position
        for (int i = 1; i < TotalMotors + 1; i++)
        {
            switch (i % 3)
            {
            case 1:
                dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, i, 30, InitPosCoxa, &dxl_error);
                dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, i, 30, &dxl_present_position, &dxl_error);

                if (dxl_comm_result != COMM_SUCCESS)
                {
                    Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
                }
                else if (dxl_error != 0)
                {
                    Serial.print(packetHandler->getRxPacketError(dxl_error));
                }
                Serial.print("[ID:");
                Serial.print(i);
                Serial.print("] GoalPos:");
                Serial.print(InitPosCoxa);
                Serial.print("  PresPos:");
                Serial.print(dxl_present_position);
                Serial.println(" ");
                break;

            case 2:
                dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, i, 30, InitPosFemur, &dxl_error);
                if (dxl_comm_result != COMM_SUCCESS)
                {
                    Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
                }
                else if (dxl_error != 0)
                {
                    Serial.print(packetHandler->getRxPacketError(dxl_error));
                }
                break;

            case 3:
                dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, i, 30, InitPosTibia, &dxl_error);
                if (dxl_comm_result != COMM_SUCCESS)
                {
                    Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
                }
                else if (dxl_error != 0)
                {
                    Serial.print(packetHandler->getRxPacketError(dxl_error));
                }
                break;

            default:
                break;
            }
        }

        delay(300);

        // Step1 Test
        // L1
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 10, 30, LCoxaStep1, &dxl_error);
        dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, 10, 30, &dxl_present_position, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }
        Serial.print("[ID:");
        Serial.print(10);
        Serial.print("] GoalPos:");
        Serial.print(LCoxaStep1);
        Serial.print("  PresPos:");
        Serial.print(dxl_present_position);
        Serial.println(" ");

        // R3
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 1, 30, RCoxaStep3, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // L1
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 11, 30, LFemurStep1, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // R3
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 2, 30, RFemurStep3, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // L1
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 12, 30, LTibiaStep1, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // R3
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 3, 30, RTibiaStep3, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        delay(300);

        // Step2 Test
        // L2
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 10, 30, LCoxaStep2, &dxl_error);
        dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, 10, 30, &dxl_present_position, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }
        Serial.print("[ID:");
        Serial.print(10);
        Serial.print("] GoalPos:");
        Serial.print(LCoxaStep2);
        Serial.print("  PresPos:");
        Serial.print(dxl_present_position);
        Serial.println(" ");

        // L2
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 11, 30, LFemurStep2, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // L2
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 12, 30, LTibiaStep2, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        delay(300);
        // Step3 Test
        // L3
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 10, 30, LCoxaStep3, &dxl_error);
        dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, 10, 30, &dxl_present_position, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }
        Serial.print("[ID:");
        Serial.print(10);
        Serial.print("] GoalPos:");
        Serial.print(LCoxaStep3);
        Serial.print("  PresPos:");
        Serial.print(dxl_present_position);
        Serial.println(" ");

        // R1
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 1, 30, RCoxaStep1, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // L3
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 11, 30, LFemurStep3, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // R1
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 2, 30, RFemurStep1, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // L3
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 12, 30, LTibiaStep3, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // R1
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 3, 30, RTibiaStep1, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // R2
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 1, 30, RCoxaStep2, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // R2
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 2, 30, RFemurStep2, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // R2
        dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, 3, 30, RTibiaStep2, &dxl_error);

        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }

        // usart command input

        // command do
    }

    // Torque disable
    for (int i = 1; i < TotalMotors + 1; i++)
    {
        dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, i, 30, TORQUE_DISABLE, &dxl_error);
        if (dxl_comm_result != COMM_SUCCESS)
        {
            Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
        }
        else if (dxl_error != 0)
        {
            Serial.print(packetHandler->getRxPacketError(dxl_error));
        }
    }

    // Close port
    portHandler->closePort();
}

// void InitLegs(void)
// {
//     for (int i = 1; i < TotalMotors + 1; i++)
//     {
//         dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, i, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
//         if (dxl_comm_result != COMM_SUCCESS)
//         {
//             Serial.print(packetHandler->getTxRxResult(dxl_comm_result));
//         }
//         else if (dxl_error != 0)
//         {
//             Serial.print(packetHandler->getRxPacketError(dxl_error));
//         }
//         else
//         {
//             Serial.print("Dynamixel has been successfully connected \n");
//         }
//     }
// }
