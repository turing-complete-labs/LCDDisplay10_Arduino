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

#define ASCII_RANGE_START 48
#define ASCII_RANGE_END 122

const uint8_t CHARACTERS[] = {
    /*  ASCII   CHAR    */
    /*  48      0       */  SEG_A | SEG_C | SEG_B | SEG_G | SEG_E | SEG_D,
    /*  49      1       */  SEG_C | SEG_B,
    /*  50      2       */  SEG_A | SEG_C | SEG_G | SEG_E | SEG_F,
    /*  51      3       */  SEG_A | SEG_C | SEG_B | SEG_G | SEG_F,
    /*  52      4       */  SEG_C | SEG_B | SEG_D | SEG_F,
    /*  53      5       */  SEG_A | SEG_B | SEG_G | SEG_D | SEG_F,
    /*  54      6       */  SEG_A | SEG_B | SEG_G | SEG_E | SEG_D | SEG_F,
    /*  55      7       */  SEG_A | SEG_C | SEG_B,
    /*  56      8       */  SEG_A | SEG_C | SEG_B | SEG_G | SEG_E | SEG_D | SEG_F,
    /*  57      9       */  SEG_A | SEG_C | SEG_B | SEG_G | SEG_D | SEG_F,
    /*  58      :       */  SEG_G | SEG_F,
    /*  59      ;       */  SEG_G | SEG_F,
    /*  60      <       */  SEG_G | SEG_E | SEG_F,
    /*  61      =       */  SEG_G | SEG_F,
    /*  62      >       */  SEG_B | SEG_G | SEG_F,
    /*  63      ?       */  SEG_A | SEG_C | SEG_E | SEG_F,
    /*  64      @       */  SEG_A | SEG_C | SEG_B | SEG_G | SEG_E | SEG_F,
    /*  65      A       */  SEG_A | SEG_C | SEG_B | SEG_E | SEG_D | SEG_F,
    /*  66      B       */  SEG_A | SEG_C | SEG_B | SEG_G | SEG_E | SEG_D | SEG_F,
    /*  67      C       */  SEG_A | SEG_G | SEG_E | SEG_D,
    /*  68      D       */  SEG_A | SEG_C | SEG_B | SEG_G | SEG_E | SEG_D,
    /*  69      E       */  SEG_A | SEG_G | SEG_E | SEG_D | SEG_F,
    /*  70      F       */  SEG_A | SEG_E | SEG_D | SEG_F,
    /*  71      G       */  SEG_A | SEG_B | SEG_G | SEG_E | SEG_D | SEG_F,
    /*  72      H       */  SEG_C | SEG_B | SEG_E | SEG_D | SEG_F,
    /*  73      I       */  SEG_E | SEG_D,
    /*  74      J       */  SEG_C | SEG_B | SEG_G,
    /*  75      K       */  SEG_C | SEG_B | SEG_E | SEG_D | SEG_F,
    /*  76      L       */  SEG_G | SEG_E | SEG_D,
    /*  77      M       */  SEG_A | SEG_E | SEG_D,
    /*  78      N       */  SEG_A | SEG_C | SEG_B | SEG_E | SEG_D,
    /*  79      O       */  SEG_A | SEG_C | SEG_B | SEG_G | SEG_E | SEG_D,
    /*  80      P       */  SEG_A | SEG_C | SEG_E | SEG_D | SEG_F,
    /*  81      Q       */  SEG_A | SEG_C | SEG_B | SEG_G | SEG_E | SEG_D | SEG_P,
    /*  82      R       */  SEG_A | SEG_C | SEG_B | SEG_E | SEG_D | SEG_F,
    /*  83      S       */  SEG_A | SEG_B | SEG_G | SEG_D | SEG_F,
    /*  84      T       */  SEG_A | SEG_E | SEG_D,
    /*  85      U       */  SEG_C | SEG_B | SEG_G | SEG_E | SEG_D,
    /*  86      V       */  SEG_C | SEG_B | SEG_G | SEG_E | SEG_D,
    /*  87      W       */  SEG_G | SEG_E | SEG_D,
    /*  88      X       */  SEG_C | SEG_B | SEG_E | SEG_D | SEG_F,
    /*  89      Y       */  SEG_C | SEG_B | SEG_G | SEG_D | SEG_F,
    /*  90      Z       */  SEG_A | SEG_C | SEG_G | SEG_E | SEG_F,
    /*  91      [       */  SEG_A | SEG_C | SEG_B | SEG_G,
    /*  92      \       */  SEG_B | SEG_D | SEG_F,
    /*  93      ]       */  SEG_A | SEG_G | SEG_E | SEG_D,
    /*  94      ^       */  SEG_A | SEG_C | SEG_D,
    /*  95      _       */  SEG_G,
    /*  96      `       */  SEG_D,
    /*  97      a       */  SEG_A | SEG_C | SEG_B | SEG_G | SEG_E | SEG_F,
    /*  98      b       */  SEG_B | SEG_G | SEG_E | SEG_D | SEG_F,
    /*  99      c       */  SEG_G | SEG_E | SEG_F,
    /*  100     d       */  SEG_C | SEG_B | SEG_G | SEG_E | SEG_F,
    /*  101     e       */  SEG_A | SEG_C | SEG_G | SEG_E | SEG_D | SEG_F,
    /*  102     f       */  SEG_A | SEG_E | SEG_D | SEG_F,
    /*  103     g       */  SEG_A | SEG_C | SEG_B | SEG_G | SEG_D | SEG_F,
    /*  104     h       */  SEG_B | SEG_E | SEG_D | SEG_F,
    /*  105     i       */  SEG_E,
    /*  106     j       */  SEG_B | SEG_G,
    /*  107     k       */  SEG_C | SEG_B | SEG_E | SEG_D | SEG_F,
    /*  108     l       */  SEG_E | SEG_D,
    /*  109     m       */  SEG_E | SEG_F,
    /*  110     n       */  SEG_B | SEG_E | SEG_F,
    /*  111     o       */  SEG_B | SEG_G | SEG_E | SEG_F,
    /*  112     p       */  SEG_A | SEG_C | SEG_E | SEG_D | SEG_F,
    /*  113     q       */  SEG_A | SEG_C | SEG_B | SEG_D | SEG_F,
    /*  114     r       */  SEG_E | SEG_F,
    /*  115     s       */  SEG_A | SEG_B | SEG_G | SEG_D | SEG_F,
    /*  116     t       */  SEG_G | SEG_E | SEG_D | SEG_F,
    /*  117     u       */  SEG_B | SEG_G | SEG_E,
    /*  118     v       */  SEG_B | SEG_G | SEG_E,
    /*  119     w       */  SEG_G | SEG_E,
    /*  120     x       */  SEG_C | SEG_B | SEG_E | SEG_D | SEG_F,
    /*  121     y       */  SEG_C | SEG_B | SEG_G | SEG_D | SEG_F,
    /*  122     z       */  SEG_A | SEG_C | SEG_G | SEG_E | SEG_F,
};

uint8_t _buffer[BUFFER_SIZE];

void LCDDisplay10::begin(TwoWire *theWire)
{
    _wire = theWire;
    _wire->begin();
    reset();
}

void LCDDisplay10::clear()
{
    fill(0);
    sendBuffer();
}

uint8_t LCDDisplay10::sendCommand(uint8_t command, uint8_t val)
{
    _wire->beginTransmission(0x38);
    _wire->write(command);
    _wire->write(val);
    return _wire->endTransmission();
}

uint8_t LCDDisplay10::writeMemory(uint8_t addr, uint8_t val)
{
    _wire->beginTransmission(0x38);
    _wire->write(0xe0);
    _wire->write(addr);
    _wire->write(val);
    return _wire->endTransmission();
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
    _wire->beginTransmission(0x38);
    _wire->write(0xe0);
    _wire->write((uint8_t)0);
    for (uint8_t i = 0; i < BUFFER_SIZE; i++)
    {
        _wire->write(_buffer[i]);
    }
    return _wire->endTransmission();
}

bool LCDDisplay10::writeToBuffer(const char *characters)
{
    int cur_pos = 0;
    int len = 0;

    uint8_t write_buffer[DIGITS];
    bool isNegative = *characters == '-';
    setNegative(isNegative);

    if (isNegative)
    {
        characters++;
    }

    while (*characters > 0)
    {
        char c = *characters;
        int c_ascii = int(c);
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
            characters++;
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
            characters++;
            continue;
        }
        if ((c_ascii >= ASCII_RANGE_START) && (c_ascii <= ASCII_RANGE_END)) {
            write_buffer[cur_pos++] = CHARACTERS[c_ascii - ASCII_RANGE_START];
            len++;
            characters++;
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

bool LCDDisplay10::print(const char *characters)
{
    if (writeToBuffer(characters))
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
                           ? (_buffer[pos] & SEG_P) | CHARACTERS[value]
                           : 0;
    }
}

void LCDDisplay10::setChar(uint8_t pos, char c)
{
    int c_ascii = int(c);

    if (pos < DIGITS)
    {
        if ((c_ascii >= ASCII_RANGE_START) && (c_ascii <= ASCII_RANGE_END)) {
            _buffer[pos] = (_buffer[pos] & SEG_P) | CHARACTERS[c_ascii - ASCII_RANGE_START];
        }
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

void LCDDisplay10::setBlink(blink_freq freq)
{
    uint8_t command = 0b01110000 | freq ;
    sendCommand(0xe0, command);
}
