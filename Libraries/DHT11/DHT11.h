#define DHT11_SUCCESS                0      // return value when reading was success
#define DHT11_NOT_REPLY              1      // return value when DHT11 did not send low signal
#define DHT11_SIGNAL_ERR             2      // return value when DHT11 did not send high signal
#define DHT11_DATA_NOT_START         3      // return value when DHT11 did not send 
#define DHT11_CHKSUM_ERR                       4      // return value when checksum is not correct
#define DHT11_DATA_HIGH_ERR          100    // return value when DHT11 signal high time was not proper
 
#define DHT11_MAX_REPLY_TIME         100                    // max repeat times to wait DHT11 to reply
#define DHT11_SIGNAL_LENGTH          (120 + 120 * 0.1)      // max repeat times for DHT11 reply 50uS
#define DHT11_DATA_LOW_LENGTH        (100 + 100 * 0.1)      // max repeat times for DHT11 low signal 50uS
#define DHT11_DATA_HIGH_LENGTH_1     DHT11_SIGNAL_LENGTH   // max repeat times for DHT11 high signal 1, 70 uS
#define DHT11_DATA_HIGH_LENGTH_0     (DHT11_DATA_HIGH_LENGTH_1 / 2)      // max repeat times for DHT11 high signal 1, 26 uS
 
#define DHT11_DATA_BITS              40
 
typedef struct {
    int16_t humid, temp;
    uint8_t checksum;
}DHT11DATA;
 
uint8_t DHT11ReadData(DHT11DATA *pData);
uint32_t WaitForLow(uint32_t max);
uint32_t WaitForHigh(uint32_t max);