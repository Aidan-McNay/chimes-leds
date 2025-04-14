// ---------------------------------------------------------------------------------
//                                CAN PARAMETERS
// ---------------------------------------------------------------------------------
// Size of TX buffer
#define MAX_PAYLOAD_SIZE        16
#define MAX_PACKET_LEN          MAX_PAYLOAD_SIZE+8
#define MAX_STUFFED_PACKET_LEN  MAX_PACKET_LEN+(MAX_PACKET_LEN>>1)

#define CORE_ARBITRATION        0x1234   
#define SENSOR_ARBITRATION      0x3234
#define NETWORK_BROADCAST       0x5555