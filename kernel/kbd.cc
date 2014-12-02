#include "kbd.h"
#include "machine.h"
#include "bb.h"
#include "debug.h"

int shift = 0;

InputStream<char> *Keyboard::is;
BB<char> *bb;

void Keyboard::init() {
    bb = new BB<char>(128);
    is = bb;
}

static unsigned char kbd_get(void);

/* interrupt handler */
void Keyboard::handler(void) {
    char ch = kbd_get();
//Debug::printf("%x\n",ch);
    bb->put(ch);
}

/* internal functions */
static unsigned char ul(char x) {
    if (shift) {
        return x - ('a' - 'A');
    } else {
        return x;
    }
}

/* get the next character from the hardware */
static unsigned char kbd_get(void) {
    while ((inb(0x64) & 1) == 0);
    uint8_t b = inb(0x60);
    const bool isUp = b & 128;
	b &= ~128;

    switch (b) {
    case 0x02 ... 0x0a : return('0' + b - 1 + (isUp ? 128 : 0));
    case 0x0b : return('0' + (isUp ? 128 : 0));
    case 0x0e: return(8 + (isUp ? 128 : 0));
    case 0x10 : return(ul('q') + (isUp ? 128 : 0));
    case 0x11 : return(ul('w') + (isUp ? 128 : 0));
    case 0x12 : return(ul('e') + (isUp ? 128 : 0));
    case 0x13 : return(ul('r') + (isUp ? 128 : 0));
    case 0x14 : return(ul('t') + (isUp ? 128 : 0));
    case 0x15 : return(ul('y') + (isUp ? 128 : 0));
    case 0x16 : return(ul('u') + (isUp ? 128 : 0));
    case 0x17 : return(ul('i') + (isUp ? 128 : 0));
    case 0x18 : return(ul('o') + (isUp ? 128 : 0));
    case 0x19 : return(ul('p') + (isUp ? 128 : 0));
    case 0x1c : return('\n' + (isUp ? 128 : 0));
    case 0x1e : return(ul('a') + ((isUp ? 128 : 0)));
    case 0x1f : return(ul('s') + (isUp ? 128 : 0));
    case 0x20 : return(ul('d') + (isUp ? 128 : 0));
    case 0x21 : return(ul('f') + (isUp ? 128 : 0));
    case 0x22 : return(ul('g') + (isUp ? 128 : 0));
    case 0x23 : return(ul('h') + (isUp ? 128 : 0));
    case 0x24 : return(ul('j') + (isUp ? 128 : 0));
    case 0x25 : return(ul('k') + (isUp ? 128 : 0));
    case 0x26 : return(ul('l') + (isUp ? 128 : 0));
    case 0x2c : return(ul('z') + (isUp ? 128 : 0));
    case 0x2d : return(ul('x') + (isUp ? 128 : 0));
    case 0x2e : return(ul('c') + (isUp ? 128 : 0));
    case 0x2f : return(ul('v') + (isUp ? 128 : 0));
    case 0x30 : return(ul('b') + (isUp ? 128 : 0));
    case 0x31 : return(ul('n') + (isUp ? 128 : 0));
    case 0x32 : return(ul('m') + (isUp ? 128 : 0));

    case 0x2a: case 0x36: shift = 1; return 0;
    case 0xaa: case 0xb6: shift = 0; return 0;
    default: return 0;
    }
 
}

