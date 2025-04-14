#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

// ---------------------------------------------------------------------------------
//                                CAN IDENTITIES
// ---------------------------------------------------------------------------------

#define CORE_ARBITRATION        0x1234   
#define SENSOR_ARBITRATION      0x3234
#define NETWORK_BROADCAST       0x5555

const short CLOCK_FACE_ARBITRATION[] = {
    0x2344,
    0x3344,
    0x4344,
    0x5344
};

// ---------------------------------------------------------------------------------
// CAN command interface IDs

typedef enum : unsigned short {
    LIGHT = 0x01, // Request light sensor data
    SOUND = 0x02, // Request sound sensor data
} sensor_msg_t;

typedef enum : unsigned short {
    COLOR = 0x03, // Set color of clock face
    INTENSITY = 0x04, // Set light intensity
} clock_face_msg_t;

#endif