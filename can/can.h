  // =======================================================================
  // can.h
  // =======================================================================
  // Declarations of the CAN bus functions

  #ifndef CAN_H
  #define CAN_H

  #include "can.pio.h"
  #include "hardware/dma.h"
  #include "hardware/irq.h"
  #include "hardware/pio.h"

  // ----------------------------------------------------------------------
  // CAN parameters
  // ----------------------------------------------------------------------

  // Size of TX buffer
  // Maximum size of payload defined in shorts
  #define MAX_PAYLOAD_SIZE 8
  #define MAX_PACKET_LEN ( MAX_PAYLOAD_SIZE + 4 )
  #define MAX_STUFFED_PACKET_LEN \
    ( MAX_PACKET_LEN + ( MAX_PACKET_LEN >> 1 ) )

  // ----------------------------------------------------------------------
  // Define clock and checksum parameters
  // ----------------------------------------------------------------------

  // Clock settings
  #define OVERCLOCK_RATE 160000
  #define CLKDIV 5
  // Checksum polynomial and initial value
  #define CRC16_POLY 0x8005
  #define CRC_INIT 0xFFFF

  // ----------------------------------------------------------------------
  // Callback type for packet handler
  // ----------------------------------------------------------------------
  typedef void ( *packet_handler_t )( const unsigned short* packet, const unsigned char len );

  // ----------------------------------------------------------------------
  // CAN Bus
  // ----------------------------------------------------------------------

  class CAN {
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Public accessor functions
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  public:
    // CAN RX is at CAN_TX+1
    CAN( unsigned short my_arbitration, unsigned short network_broadcast, int can_tx, int transceiver_en );

    void set_callback ( packet_handler_t handler )
    {
      packet_handler = handler;
    }

    void set_my_arbitration( unsigned short my_arbitration )
    {
      this->my_arbitration = my_arbitration;
    }
    void set_arbitration( unsigned short arbitration )
    {
      this->arbitration = arbitration;
    }
    void set_network_broadcast( unsigned short network_broadcast )
    {
      this->network_broadcast = network_broadcast;
    }
    void set_payload( unsigned short* new_payload, unsigned char len )
    {
      payload_len = len;
      for ( int i = 0; i < len; i++ ) {
        payload[i] = new_payload[i];
      }
    }
    unsigned short get_my_arbitration()
    {
      return my_arbitration;
    }
    unsigned short get_arbitration()
    {
      return arbitration;
    }
    unsigned short get_network_broadcast()
    {
      return network_broadcast;
    }

    // User interrupt service routine
    void handle_tx();
    void handle_rx();

    void set_number_sent( volatile int number_sent )
    {
      this->number_sent = number_sent;
    }
    void set_number_received( volatile int number_received )
    {
      this->number_received = number_received;
    }
    void set_number_missed( volatile int number_missed )
    {
      this->number_missed = number_missed;
    }
    int get_number_sent()
    {
      return number_sent;
    }
    int get_number_received()
    {
      return number_received;
    }
    int get_number_missed()
    {
      return number_missed;
    }

    // Computes the checksum
    unsigned short culCalcCRC( char crcData, unsigned short crcReg );
    
    // Packet stuffing
    unsigned short getBitShort( unsigned short* shorty,
                                unsigned char   bitnum );
    void modifyBitShort( unsigned short* shorty, unsigned char bitnum,
                        unsigned short value );

    // Packet transmission
    void bitStuff( unsigned short* unstuffed, unsigned short* stuffed );
    void sendPacket();

    // Packet reception
    void unBitStuff( unsigned short* stuffed, unsigned short* unstuffed );
    unsigned char attemptPacketReceive();

    // Driver interrupt service routine (ISR)
    static void
    dma_handler();  // overrun on the RX DMA channel, then being reset

    // Setup CAN bus
    void setupIdleCheck();
    void setupCANTX( irq_handler_t handler );
    void setupCANRX( irq_handler_t handler );

    // API helper functions
    static inline void resetTransmitter();
    static inline void resetReceiver();
    static inline void acceptNewPacket();

  protected:
    packet_handler_t packet_handler;  // Pointer to the packet handler function
    int can_tx, transceiver_en; // GPIOs for CAN interface
    unsigned short my_arbitration, arbitration, network_broadcast;
    unsigned int tx_idle_time;  // time to wait (in bit times) for bus to be
                                // idle before TX
    unsigned char  reserve_byte;  // reserve byte
    unsigned char  payload_len;   // payload length in bytes (even)
    unsigned short payload[MAX_PAYLOAD_SIZE] = { 0x1335, 0x5678, 0x9012,
                                                0x3456, 0x7890 };

    volatile int number_sent;      // # of sent messages
    volatile int number_received;  // # of received messages
    volatile int number_missed;    // # of rejected packets
    volatile int unsafe_to_tx;  // flag for indicating that it is unsafe to transmit
  };

  #endif  // CAN_H