#include "keyboard.h"
#include "interrupt.h"
#include "commons.h"
#include "io.h"
#include "pic.h"
#include "io/kio.h"

/**********************************************************
* Private Kernel variable declarations                    *
**********************************************************/

static unsigned char is_lshift_down       = 0;
static unsigned char is_rshift_down       = 0;
static unsigned char is_caps_lock_pressed = 0;

static struct kbd_buffer kbd_buf;

/**********************************************************
* Private Kernel function declarations                    *
**********************************************************/

/**
* keyboard_interrupt_handler:
* interrupt handler of keyboard
*/
void keyboard_interrupt_handler(struct cpu_state state,
                                struct idt_info info,
                                struct stack_state exec);

/** kbd_read_scan_code:
 *  Reads a scan code from the keyboard
 *
 *  @return The scan code (NOT an ASCII character!)
 */
unsigned char kbd_read_scan_code(void);

char kbd_scan_code_to_ascii(unsigned char sc);
static void toggle_left_shift(void);
static void toggle_right_shift(void);
static void toggle_caps_lock(void);
static char handle_caps_lock(unsigned char ch);
static char handle_shift(unsigned char ch);

/**********************************************************
* Public Kernel API functions                             *
**********************************************************/

unsigned int kbd_init(void)
{
  register_interrupt_handler(KBD_INT_IDX, (interrupt_handler_t)&keyboard_interrupt_handler);
  kbd_buf.count = 0;
  kbd_buf.head = kbd_buf.buffer;
  kbd_buf.tail = kbd_buf.buffer;
  return (0);
}

int        kbd_read(void *buf, size_t count)
{
  char ch;
  int i = 0;
  char *b = buf;

  while (count > 0)
  {
    if (kbd_buf.head == kbd_buf.tail)
      return i;
    while (kbd_buf.head != kbd_buf.tail)
    {
      ch = kbd_scan_code_to_ascii(*kbd_buf.head++);
      if (kbd_buf.head == kbd_buf.buffer + KBD_BUFFER_SIZE)
      {
        kbd_buf.head = kbd_buf.buffer;
      }
      if (ch != -1)
      {
        b[i] = ch;
        ++i;
        --count;
        kbd_buf.count--;
      }
    }
  }
  return i;
}

/**********************************************************
 * Private Kernel functions                               *
 **********************************************************/

 void keyboard_interrupt_handler(struct cpu_state state,
                                 struct idt_info info,
                                 struct stack_state exec)
 {
   UNUSED_ARG(state);
   UNUSED_ARG(info);
   UNUSED_ARG(exec);

   if (kbd_buf.count < KBD_BUFFER_SIZE)
   {
      *kbd_buf.tail++ = kbd_read_scan_code();
      kbd_buf.count += 1;
      if (kbd_buf.tail == kbd_buf.buffer + KBD_BUFFER_SIZE)
      {
        kbd_buf.tail = kbd_buf.buffer;
      }
   }
   pic_acknowledge();
 }

unsigned char kbd_read_scan_code(void)
{
 return inb(KBD_DATA_PORT);
}

static void toggle_left_shift(void)
{
    is_lshift_down = is_lshift_down ? 0 : 1;
}

static void toggle_right_shift(void)
{
    is_rshift_down = is_rshift_down ? 0 : 1;
}

static void toggle_caps_lock(void)
{
    is_caps_lock_pressed = is_caps_lock_pressed ? 0 : 1;
}

static char handle_caps_lock(unsigned char ch)
{
    if (ch >= 'a' && ch <= 'z') {
        return ch + 'A' - 'a';
    }
    return ch;
}

static char handle_shift(unsigned char ch)
{
    /* Alphabetic characters */
    if (ch >= 'a' && ch <= 'z') {
        return ch + 'A' - 'a';
    }

    /* Number characters */
    switch (ch) {
        case '0':
            return ')';
        case '1':
            return '!';
        case '2':
            return '@';
        case '3':
            return '#';
        case '4':
            return '$';
        case '5':
            return '%';
        case '6':

            return '^';
        case '7':
            return '&';
        case '8':
            return '*';
        case '9':
            return '(';
        default:
            break;
    }

    /* Special charachters */
    switch (ch) {
        case '-':
            return '_';
        case '=':
            return '+';
        case '[':
            return '{';
        case ']':
            return '}';
        case '\\':
            return '|';
        case ';':
            return ':';
        case '\'':
            return '\"';
        case ',':
            return '<';
        case '.':
            return '>';
        case '/':
            return '?';
        case '`':
            return '~';
          }
    return ch;
}

 char kbd_scan_code_to_ascii(unsigned char scan_code)
 {
   char ch = -1;

    if (scan_code & 0x80) {
        scan_code &= 0x7F; /* clear the bit set by key break */

        switch (scan_code) {
            case KBD_SC_LSHIFT:
                toggle_left_shift();
                break;
            case KBD_SC_RSHIFT:
                toggle_right_shift();
                break;
            case KBD_SC_CAPSLOCK:
                toggle_caps_lock();
                break;
            default:
                break;
        }

        return -1;
    }
    switch (scan_code) {
        case KBD_SC_A:
            ch = 'a';
            break;
        case KBD_SC_B:
            ch = 'b';
            break;
        case KBD_SC_C:
            ch = 'c';
            break;
        case KBD_SC_D:
            ch = 'd';
            break;
        case KBD_SC_E:
            ch = 'e';
            break;
        case KBD_SC_F:
            ch = 'f';
            break;
        case KBD_SC_G:
            ch = 'g';
            break;
        case KBD_SC_H:
            ch = 'h';
            break;
        case KBD_SC_I:
            ch = 'i';
            break;
        case KBD_SC_J:
            ch = 'j';
            break;
        case KBD_SC_K:
            ch = 'k';
            break;
        case KBD_SC_L:
            ch = 'l';
            break;
        case KBD_SC_M:
            ch = 'm';
            break;
        case KBD_SC_N:
            ch = 'n';
            break;
        case KBD_SC_O:
            ch = 'o';
            break;
        case KBD_SC_P:
            ch = 'p';
            break;
        case KBD_SC_Q:
            ch = 'q';
            break;
        case KBD_SC_R:
            ch = 'r';
            break;
        case KBD_SC_S:
            ch = 's';
            break;
        case KBD_SC_T:
            ch = 't';
            break;
        case KBD_SC_U:
            ch = 'u';
            break;
        case KBD_SC_V:
            ch = 'v';
            break;
        case KBD_SC_W:
            ch = 'w';
            break;
        case KBD_SC_X:
            ch = 'x';
            break;
        case KBD_SC_Y:
            ch = 'y';
            break;
        case KBD_SC_Z:
            ch = 'z';
            break;
        case KBD_SC_0:
            ch = '0';
            break;
        case KBD_SC_1:
            ch = '1';
            break;
        case KBD_SC_2:
            ch = '2';
            break;
        case KBD_SC_3:
            ch = '3';
            break;
        case KBD_SC_4:
            ch = '4';
            break;
        case KBD_SC_5:
            ch = '5';
            break;
        case KBD_SC_6:
            ch = '6';
            break;
        case KBD_SC_7:
            ch = '7';
            break;
        case KBD_SC_8:
            ch = '8';
            break;
        case KBD_SC_9:
            ch = '9';
            break;
        case KBD_SC_ENTER:
            ch = '\n';
            break;
        case KBD_SC_SPACE:
            ch = ' ';
            break;
        case KBD_SC_BS:
            ch = 8;
            break;
        case KBD_SC_DASH:
            ch = '-';
            break;
        case KBD_SC_EQUALS:
            ch = '=';
            break;
        case KBD_SC_LBRACKET:
            ch = '[';
            break;
        case KBD_SC_RBRACKET:
            ch = ']';
            break;
        case KBD_SC_BSLASH:
            ch = '\\';
            break;
        case KBD_SC_SCOLON:
            ch = ';';
            break;
        case KBD_SC_QUOTE:
            ch = '\'';
            break;
        case KBD_SC_COMMA:
            ch = ',';
            break;
        case KBD_SC_DOT:
            ch = '.';
            break;
        case KBD_SC_FSLASH:
            ch = '/';
            break;
        case KBD_SC_TILDE:
            ch = '`';
            break;
        case KBD_SC_TAB:
            ch = '\t';
            break;
        case KBD_SC_LSHIFT:
            toggle_left_shift();
            break;
        case KBD_SC_RSHIFT:
            toggle_right_shift();
            break;
        default:
            return -1;
    }

    if (is_caps_lock_pressed) {
        ch = handle_caps_lock(ch);
    }

    if (is_lshift_down || is_rshift_down) {
        ch = handle_shift(ch);
    }

    return ch;
 }
