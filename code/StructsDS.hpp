#include <inttypes.h>

#define GCCPACKED __attribute__ ((packed))

#define DS2PacketSize 21
#define DS4PacketUSBSize 64

enum DS2Mode: uint8_t {
    DS2Mode_Digital = 0x4,
    DS2Mode_Analog = 0x7,
    DS2Mode_Config = 0xF
};

enum DS2Command: uint8_t {
    /*
    Notes: The controller can be configured (through command 0x4F) to respond with more or less information about each button with each poll. Only works when the controller is already in configuration mode (0xF3) use Command 0x43 to enter / exit configuration mode.
    Descriptions: 
    Find out what buttons are included in poll responses (Bits corresponding to buttons in response packet).
    18 total bytes can be turned on or off, including the 2 digital state bytes and 16 analog bytes (pressures and joysticks).
    Byte 9 of command and byte 9 of data are always 0x5A.
    Data is all 0x00 if controller is in digital mode (0x41).
    Command data is always 0x5A, although 0x00 yields the same result.
    */
    DS2Command_GetButtonMask = 0x41,

    /*
    Notes: Depending on the controller's configuration, this command can get all the digital and analog button states, as well as control the vibration motors.
    Descriptions: 
    If mode (Byte 2 in data) is 0x41, the packet only contains 5 bytes, if mode == 0x73, 9 bytes are returned.
    WW and YY are used to control the motors (which does what depends on the config).
    */
    DS2Command_ExclusivePoll = 0x42,

    /*
    Notes: This can poll the controller like 0x42, but if the first command byte is 1, it has the effect of entering config mode (0xF3), in which the packet response can be configured. If the current mode is 0x41, this command only needs to be 5 bytes long. Once in config / escape mode, 0x43 does not return button states anymore, but 0x42 still does (except for pressures). Also, all packets have will 6 bytes of command / data after the header.
    Descriptions:
    Byte 4 in command = 0x01 enters config mode (or "escape" mode at Dowty calls it)., 0x00 exits. If 0x00 and already out of config mode, it behaves just like 0x42, but without vibration motor control.
    */
    DS2Command_ToggleConfigAndPoll = 0x43,

    /*
    Notes: Only works if the controller is in config mode (0xF3).
    Descriptions:
    Set analog mode: Byte 4 in command = 0x01, set digital mode: Byte 4 in command = 0x00
    If Byte 5 in command is 0x03, the controller is locked, otherwise the user can change from analog to digital mode using the analog button on the controller. Note that if the controller is already setup to deliver pressure values, toggling the state with the controller button will revert the controller back into not sending pressures.
    Some controllers have a watch-dog timer that reverts back into digital mode if a command is not received within a second or so.
    */
    DS2Command_ToggleAnalogDigitalMode = 0x44,

    /*
    Notes: Only works after the controller is in config mode (0xF3).
    Descriptions:
    Byte 4 in data = 0x03 for dual shock controller, 0x01 for guitar hero
    Byte 6 in data = 0x01 when LED is on, 0x00 when it's off
    */
    DS2Command_ToggleConfigMode = 0x45,

    /*
    Notes: This command is always issued twice in a row, and appears to be retrieving a 10 byte constant of over those two calls. It is always called in a sequence of 0x46 0x46 0x47 0x4C 0x4C. Only works after the controller is in config mode (0xF3).
    Descriptions:
    Byte 4 in command appears to get the first half of the constant when it's 0x00, and the 2nd half when it's 0x01.
    As shown above, the constant returned for a dual shock controller is: 00 00 02 00 0A    00 00 00 00 14
    A Katana wireless controller and the guitar hero controller each returned this: 00 01 02 00 0A    00 01 01 01 14
    */
    DS2Command_GetConstant1 = 0x46,

    /*
    Notes: It is always called in a command sequence of 0x46 0x46 0x47 0x4C 0x4C. Only works after the controller is in config mode (0xF3).
    Descriptions:
    Dowty thinks the first byte is probably an offset like in 0x46 and 0x4C, which would leave 5 bytes of interest.
    As shown above, the constant returned for a dual shock controller is: 00 02 00 00 00
    guitar hero controller and Katana wireless: 00 02 00 01 00
    */
    DS2Command_GetConstant2 = 0x47,

    /*
    Notes: This command is always issued twice in a row, and appears to be retrieving a 10 byte constant of over those two calls. It is always called in a command sequence of 0x46 0x46 0x47 0x4C 0x4C. Only works after the controller is in config mode (0xF3).
    Descriptions:
    Byte 4 in command appears to get the first half of the constant when it's 0x00, and the 2nd half when it's 0x01.
    As shown above, the constant returned for a dual shock controller is: 00 00 04 00 00    00 00 06 00 00
    A Katana wireless controller and the guitar hero controller each returned this: 00 00 04 00 00    00 00 07 00 00
    */
    DS2Command_GetConstant3 = 0x4C,

    /*
    Notes: Only works after the controller is in config mode (0xF3).
    Descriptions:
    0x00 maps the corresponding byte in 0x42 to control the small motor. A 0xFF in the 0x42 command will turn it on, all other values turn it off.
    0x01 maps the corresponding byte in 0x42 to control the large motor. The power delivered to the large motor is then set from 0x00 to 0xFF in 0x42. 0x40 was the smallest value that would actually make the motor spin for us.
    0xFF disables, and is the default value when the controller is first connected. The data bytes just report the current mapping.
    Things don't always work if more than one command byte is mapped to a motor.
    */
    DS2Command_MapActuateVibrationMotorsBytesInPollCommand = 0x4D,

    /*
    Notes: This could set up the controller to only reply with the L1 and R1 pressures for instance. Only works after the controller is in config mode (0xF3).
    Descriptions:
    Each of the 18 bits from byte 4 correspond to a response byte, starting with the digital states, then 4 analog joysticks, then 12 pressure bytes.
    By default, the pressure values are not sent back, so this is the command that is necessary to enable them.
    */
    DS2Command_MaskAnalogResponseInMainPollCommand = 0x4F
};

// Caution heressy ahead

#pragma pack(push, r1, 1)

struct BitField8{
    bool b0: 1;
    bool b1: 1;
    bool b2: 1;
    bool b3: 1;
    bool b4: 1;
    bool b5: 1;
    bool b6: 1;
    bool b7: 1;
} GCCPACKED;

static_assert(sizeof(BitField8) == sizeof(uint8_t), "Bitfield size mismatch.");

struct BitField16 : BitField8{
    bool b8: 1;
    bool b9: 1;
    bool b10: 1;
    bool b11: 1;
    bool b12: 1;
    bool b13: 1;
    bool b14: 1;
    bool b15: 1;
} GCCPACKED;

static_assert(sizeof(BitField16) == sizeof(uint16_t), "Bitfield size mismatch.");

struct BitField32 : BitField16{
    bool b16: 1;
    bool b17: 1;
    bool b18: 1;
    bool b19: 1;
    bool b20: 1;
    bool b21: 1;
    bool b22: 1;
    bool b23: 1;
    bool b24: 1;
    bool b25: 1;
    bool b26: 1;
    bool b27: 1;
    bool b28: 1;
    bool b29: 1;
    bool b30: 1;
    bool b31: 1;
} GCCPACKED;

static_assert(sizeof(BitField32) == sizeof(uint32_t), "Bitfield size mismatch.");

struct BitField64 : BitField32{
    bool b32: 1;
    bool b33: 1;
    bool b34: 1;
    bool b35: 1;
    bool b36: 1;
    bool b37: 1;
    bool b38: 1;
    bool b39: 1;
    bool b40: 1;
    bool b41: 1;
    bool b42: 1;
    bool b43: 1;
    bool b44: 1;
    bool b45: 1;
    bool b46: 1;
    bool b47: 1;
    bool b48: 1;
    bool b49: 1;
    bool b50: 1;
    bool b51: 1;
    bool b52: 1;
    bool b53: 1;
    bool b54: 1;
    bool b55: 1;
    bool b56: 1;
    bool b57: 1;
    bool b58: 1;
    bool b59: 1;
    bool b60: 1;
    bool b61: 1;
    bool b62: 1;
    bool b63: 1;
} GCCPACKED;

static_assert(sizeof(BitField64) == sizeof(uint64_t), "Bitfield size mismatch.");

struct DS2Packet{
    union{
        // Command part of the packet PS2 -> Controller
        struct{
            // Report ID starts with 0x01 for controller
            uint8_t reportID;

            // Can either poll controller or configure it.
            DS2Command mainCommand;

            // Constant: Always 0x00
            uint8_t constant;

            // Can be configured to control either of the motors
            uint8_t vibMotorSel0;

            // Can be configured to control either of the motors
            uint8_t vibMotorSel1;

            // Unfunctional bytes
            uint8_t padding[16];
        } command GCCPACKED;

        // Data part of the packet Controller -> PS2
        struct{
            // Constant: Always 0xFF
            uint8_t constant;
        
            // Size: How many 16 bit ints after the header
            uint8_t size: 4;

            // Device Mode: 0x4 is digital - 0x7 is analog - 0xF config / escape?
            DS2Mode mode: 4;

            // Constant: Always 0x5A, this value appears in several non-functional places
            uint8_t constant2;

            // Digital button states (In modes: DS2Mode_Digital (0x4), DS2Mode_Analog(0x7))
            bool select: 1;
            bool l3: 1;
            bool r3: 1;
            bool start: 1;
            bool dPadup: 1;
            bool dPadRight: 1;
            bool dPadDown: 1;
            bool dPadLeft: 1;
            bool l2: 1;
            bool r2: 1;
            bool l1: 1;
            bool r1: 1;
            bool triangle: 1;
            bool circle: 1;
            bool corss: 1;
            bool square: 1;

            // Analog sticks states: max = 0x7F (In modes: DS2Mode_Analog(0x7))
            int8_t rStickX;
            int8_t rStickY;
            int8_t lStickX;
            int8_t lStickY;

            // Button pressure data (In modes: DS2Mode_Analog(0x7))
            uint8_t dPadRightPressure;
            uint8_t dPadLeftPressure;
            uint8_t dPadUpPressure;
            uint8_t dPadDownPressure;
            uint8_t trianglePressure;
            uint8_t circlePressure;
            uint8_t crossPressure;
            uint8_t squarePressure;
            uint8_t l1Pressure;
            uint8_t r1Pressure;
            uint8_t l2Pressure;
            uint8_t r2Pressure;
        } data GCCPACKED;
    } GCCPACKED;

    // Get by index
	template<typename T = uint8_t>
	T& operator[](const size_t index) const {
		return *(reinterpret_cast<T*>(this) + index);
	}	

	template<typename T = uint8_t>
	T& get(const size_t index) const {
		return operator[]<T>(index);
	}

    // Set all the memory of the packet to 0
    void Clear(){
        DS2Packet clr = { 0 };
        *this = clr;
    }
} GCCPACKED;

static_assert(sizeof(DS2Packet) == DS2PacketSize, "DS4 Packet defined struct does not match the buffer size.");

struct DS4PacketUSB{
    // Report ID
    uint8_t  reportID;
    
    // Sticks stats
    int8_t   lStickX;
    int8_t   lStickY;
    int8_t   rStickX;
    int8_t   rStickY;

    // D-Pad: (Hat format) stats 0=N 1=NE 2=E 3=SE 4=S 5=SW 6=W 7=NW
    uint8_t  dPad: 4;
    
    // Square Cross Circle Triangle stats
    bool     square: 1;
    bool     cross: 1;
    bool     circle: 1;
    bool     triangle: 1;
    
    // Shoulder buttons stats
    bool     l1: 1;
    bool     r1: 1;
    bool     l2: 1;
    bool     r2: 1; 
    
    // Mid buttons stats
    bool     share: 1;
    bool     options: 1;
    bool     l3: 1;
    bool     r3: 1;
    bool     psButton: 1;
    bool     tPadClick: 1;

    // Counter
    uint8_t  counter: 4;
    
    // L2 R2 pressure value
    uint8_t  l2Pressure;
    uint8_t  r2Pressure;

    // Seems to be a timestamp. A common increment value between two reports is 188 (at full rate the report period is 1.25ms). This timestamp is used by the PS4 to process acceleration and gyroscope data. 
    uint16_t timeStamp;
    
    // Battery level
    uint8_t  batteryLevel;
    
    // Gyro: angular velocity measures (follows right-hand-rule)
    uint16_t gyroX;
    uint16_t gyroY;
    uint16_t gyroZ;

    // Accelerometer: Acceleration in 3 directions
    int16_t  accelX; // Signed: acceleration (positive: right)
    int16_t  accelY; // Signed: acceleration (positive: up)
    int16_t  accelZ; // Signed: acceleration (positive: towards player)

    // Unknown
    uint32_t unknown0;
    
    // Accessories
    uint8_t  accessories; // 01111011 is headset with mic (0x7B) - 00111011 is headphones (0x3B) - 00011011 is nothing attached (0x1B) - 00001000 is bluetooth? (0x08) - 00000101 is ? (0x05)
    
    // Unknown
    uint16_t unknown1; // Speculation: theses 5 next (reserved) nibbles for future additional products
    
    // T-PAD: event active: (seen only)
    uint8_t  tPadEventActive: 4; // 0x00: No data for T-PAD - 0x01: Set data for 2 current touches - 0x02: set data for previous touches at bytes [44-51]
    
    // Unknow
    uint8_t  unknown2: 4; // Unknown: speculation: could be bitmaps for control commands like volume, etc.
    
    // T-PAD: incremental number
    uint8_t  tPadIncNum; // Auto incrementing number to track last update? 
    
    // T-PAD: tracking finger N1
    uint32_t tPadN1FinTrackNum: 23; //T-PAD: tracking numbers, unique to each finger (N1) down, so for each lift and repress, it gets a newly incremented figure.
    bool     tPadN1FinDown: 1; // 0 if finger N1 is down
    
    // T-PAD: each finger (N1) location/positional data: static upon finger lifting, to maintain state.
    uint8_t  tPadN1FinLocs; // To decode, each coordinated (x & y) is using 12 bits, you need to mask/split and swap the middle byte : e.g: 0x8a 4|0 28 -> 0x08a 284 -> x= 138 y= 644 
    
    // T-PAD: tacking finger N2
    uint32_t tPadN2FinTrackNum: 23; //T-PAD: tracking numbers, unique to each finger (N1) down, so for each lift and repress, it gets a newly incremented figure.
    bool     tPadN2FinDown: 1; // 0 if finger N1 is down
    
    // T-PAD: each finger (N2) location/positional data: static upon finger lifting, to maintain state.
    uint8_t  tPadN2FinLocs; // To decode, each coordinated (x & y) is using 12 bits, you need to mask/split and swap the middle byte : e.g: 0x8a 4|0 28 -> 0x08a 284 -> x= 138 y= 644 
    
    // T-PAD: the previous touches (N1) track and location
    uint32_t tPadPrevN1FinLocs: 24;

    // T-PAD: the previous touches (N2) track and location
    uint32_t tPadPrevN2FinLocs: 24;

    // Unknown
    uint8_t unknown3[10]; // TODO

	// Get by index
	template<typename T = uint8_t>
	T& operator[](const size_t index) const {
		return *(reinterpret_cast<T*>(this) + index);
	}	

	template<typename T = uint8_t>
	T& get(const size_t index) const {
		return operator[]<T>(index);
	}

    // Set all the memory of the packet to 0
    void Clear(){
        DS4PacketUSB clr = { 0 };
        *this = clr;
    }
} GCCPACKED;

static_assert(sizeof(DS4PacketUSB) == DS4PacketUSBSize, "DS4 Packet defined struct does not match the buffer size.");

#pragma pack(pop, r1)

// Read a bit
bool ReadBit(void* address, size_t index){
    // Calculate the absolute address of the byte contating the bit
    uint8_t* bytePtr = reinterpret_cast<uint8_t*>(address) + (index / 8);
    // Index of the bit in the byte
    auto bit = index % 8;

    return (*bytePtr >> bit) & 0b00000001;
}

// Writes to a bit
void WriteBit(void* address, size_t index, bool value = true){
    // Calculate the absolute address of the byte contating the bit
    uint8_t* bytePtr = reinterpret_cast<uint8_t*>(address) + (index / 8);
    // Index of the bit in the byte
    auto bit = index % 8;
    
    if(value){
        // Setting the bit if value is true
        *bytePtr |= (0b00000001 << bit);
    } else {
        // Clearing the bit if value is false
        *bytePtr &= ~(0b00000001 << bit);
    }
}

// Returns a bitfield 
template<typename T = BitField8>
T& GetBitField(void* address){
    static_assert(std::is_base_of<BitField8, T>::value, "Template argument must be a BitField8/16/32/64.");
    return *reinterpret_cast<T*>(address);
}
