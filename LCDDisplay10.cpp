#include <LCDDisplay10.h>
#include <Wire.h>

#define PCF_CONTINUE (1 << 7)
//     AAA
//    D   C
//    D   C
//     FFF
//    E   B
//    E   B
//     GGG  PP

#define SEG_A 0b10000000
#define SEG_B 0b01000000
#define SEG_C 0b00100000
#define SEG_D 0b00001000
#define SEG_E 0b00000100
#define SEG_F 0b00000010
#define SEG_G 0b00000001

#define SEG_P 0b00010000

// The following segments are mapped to location 10 and 11
// following this format (* means unused bit):
//   T1 Min Mem Err T2 T4 T3 * T7 T5 T6 * * * * *
#define SEG_Err 0b00010000
#define SEG_Mem 0b00100000
#define SEG_Min 0b01000000

#define TS_SIZE 7
const uint16_t TS_SEGMENTS[TS_SIZE] = {
    0b1000000000000000,
    0b0000100000000000,
    0b0000001000000000,
    0b0000010000000000,
    0b0000000001000000,
    0b0000000000100000,
    0b0000000010000000};

const uint8_t TS_INDEX[TS_SIZE] = {T_1, T_2, T_3, T_4, T_5, T_6, T_7};

#define BUFFER_SIZE 12
#define DIGITS 10
#define FLAGS 10

const uint8_t DIGIT_SEGMENTS[] = {
    SEG_A | SEG_C | SEG_B | SEG_G | SEG_E | SEG_D,
    SEG_C | SEG_B,
    SEG_A | SEG_C | SEG_G | SEG_E | SEG_F,
    SEG_A | SEG_C | SEG_B | SEG_G | SEG_F,
    SEG_C | SEG_B | SEG_D | SEG_F,
    SEG_A | SEG_B | SEG_G | SEG_D | SEG_F,
    SEG_A | SEG_B | SEG_G | SEG_E | SEG_D | SEG_F,
    SEG_A | SEG_C | SEG_B,
    SEG_A | SEG_C | SEG_B | SEG_G | SEG_E | SEG_D | SEG_F,
    SEG_A | SEG_C | SEG_B | SEG_G | SEG_D | SEG_F};

uint8_t _buffer[BUFFER_SIZE];

void LCDDisplay10::begin(uint8_t sda, uint8_t scl)
{
    Wire.begin();
    reset();
}

void LCDDisplay10::clear()
{
    fill(0);
    sendBuffer();
}

uint8_t LCDDisplay10::sendCommand(uint8_t command, uint8_t val)
{
    Wire.beginTransmission(0x038);
    Wire.write(command);
    Wire.write(val);
    return Wire.endTransmission();
}

uint8_t LCDDisplay10::writeMemory(uint8_t addr, uint8_t val)
{
    Wire.beginTransmission(0x38);
    Wire.write(0xe0);
    Wire.write(addr);
    Wire.write(val);
    return Wire.endTransmission();
}

void LCDDisplay10::fill(uint8_t c)
{
    for (uint8_t i = 0; i < BUFFER_SIZE; i++)
    {
        _buffer[i] = c;
    }
}

void LCDDisplay10::fillDigits(uint8_t c)
{
    for (uint8_t i = 0; i < DIGITS; i++)
    {
        _buffer[i] = c;
    }
}

uint8_t LCDDisplay10::sendBuffer()
{
    Wire.beginTransmission(0x38);
    Wire.write(0xe0);
    Wire.write((uint8_t)0);
    for (uint8_t i = 0; i < BUFFER_SIZE; i++)
    {
        Wire.write(_buffer[i]);
    }
    return Wire.endTransmission();
}

bool LCDDisplay10::writeToBuffer(const char *number)
{
    int cur_pos = 0;
    int len = 0;

    uint8_t write_buffer[DIGITS];
    bool isNegative = *number == '-';
    setNegative(isNegative);

    if (isNegative)
    {
        number++;
    }

    while (*number > 0)
    {
        char c = *number;
        if (c == '.')
        {
            if (!cur_pos)
            {
                // leading '.'
                write_buffer[cur_pos] = SEG_P;
            }
            else
            {
                write_buffer[cur_pos - 1] |= SEG_P;
            }
            number++;
            continue;
        }
        if (len >= 10)
        {
            break;
        }
        if (c == ' ')
        {
            write_buffer[cur_pos++] = 0;
            len++;
            number++;
            continue;
        }
        if ((c >= '0') && (c <= '9'))
        {
            write_buffer[cur_pos++] = DIGIT_SEGMENTS[c - '0'];
            len++;
            number++;
            continue;
        }
        return false; // for now
    }
    // clear the display buffer
    fillDigits(0);
    for (uint8_t i = 0; i < len; i++)
    {
        _buffer[10 - len + i] = write_buffer[len - i - 1];
    }
    return true;
}

void LCDDisplay10::reset()
{
    sendCommand(0xe0, 0x48);
    delay(3);
    sendCommand(0xe0, 0x70);
}

bool LCDDisplay10::print(const char *number)
{
    if (writeToBuffer(number))
    {
        sendBuffer();
        return true;
    };
    return false;
}

void LCDDisplay10::setError(bool errorFlag)
{
    _buffer[FLAGS] = errorFlag ? _buffer[FLAGS] | SEG_Err : _buffer[FLAGS] & ~SEG_Err;
}

void LCDDisplay10::setMemory(bool memoryFlag)
{
    _buffer[FLAGS] = memoryFlag ? _buffer[FLAGS] | SEG_Mem : _buffer[FLAGS] & ~SEG_Mem;
}

void LCDDisplay10::setNegative(bool negativeFlag)
{
    _buffer[FLAGS] = negativeFlag ? _buffer[FLAGS] | SEG_Min : _buffer[FLAGS] & ~SEG_Min;
}

void LCDDisplay10::setDigit(uint8_t pos, int8_t value)
{
    if (pos < DIGITS)
    {
        _buffer[pos] = value >= 0 && value <= 9
                           // preserve decimal pos
                           ? (_buffer[pos] & SEG_P) | DIGIT_SEGMENTS[value]
                           : 0;
    }
}

void LCDDisplay10::setPointPos(uint8_t pos)
{
    if (pos < DIGITS)
    {
        for (uint8_t dp = 0; dp < DIGITS; dp++)
        {
            // Clear all other decimal points.
            _buffer[dp] = _buffer[dp] & ~SEG_P;
        }
        _buffer[pos] |= SEG_P;
    }
}

void LCDDisplay10::setThousands(uint8_t n)
{
    uint16_t onBits = 0;
    uint16_t offBits = 0;

    for (int i = 0; i < TS_SIZE; i++)
    {
        if (TS_INDEX[i] & n)
        {
            onBits |= TS_SEGMENTS[i];
        }
        else
        {
            offBits |= TS_SEGMENTS[i];
        }
    }

    offBits = ~offBits;
    uint8_t oldValue = _buffer[FLAGS];
    _buffer[FLAGS] = (oldValue | (onBits >> 8)) & (offBits >> 8);
    oldValue = _buffer[FLAGS + 1];
    _buffer[FLAGS + 1] = (oldValue | (onBits & 0xff)) & (offBits & 0xff);
}
