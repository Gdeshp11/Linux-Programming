/*Mouse simulation using uinput kernel module 
* Mouse Operations Supported: 
* 1. Mouse left/right click 
* 2. Mouse Cursor Movement Up/Down/Left/Right
* 3. Press and Hold Mouse left click
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <time.h>
#include <stdbool.h>

#define ERROR(str, args...) \
    do                      \
    {                       \
        perror(str);        \
        exit(EXIT_FAILURE); \
    } while (0)

typedef enum
{
    RELEASE_KEY = 0,
    PRESS_KEY
} KEY_ACTION;

int report_input_event(int fd)
{
    struct input_event report_ev;
    report_ev.type = EV_SYN;
    report_ev.code = SYN_REPORT;
    report_ev.value = 0;
    int ret = write(fd, &report_ev, sizeof(struct input_event));
    return ret;
}

int hold_mouse_button_pressed(int fd, bool left, bool right)
{
    int ret;
    struct input_event mouse_ev;
    mouse_ev.type = EV_KEY;
    mouse_ev.code = left ? BTN_LEFT : BTN_RIGHT;
    mouse_ev.value = PRESS_KEY;
    ret = write(fd, &mouse_ev, sizeof(struct input_event));
    if (ret < 0)
        ERROR("write error: mouse click press");
    if (report_input_event(fd) < 0)
        ERROR("write error: report event");
    return ret;
}

/* set left/right value to true/false for left/right button click */
int click_mouse_button(int fd, bool left, bool right)
{
    int ret;
    struct input_event mouse_ev;
    mouse_ev.type = EV_KEY;
    mouse_ev.code = left ? BTN_LEFT : BTN_RIGHT;
    mouse_ev.value = PRESS_KEY;
    ret = write(fd, &mouse_ev, sizeof(struct input_event));
    if (ret < 0)
        ERROR("write error: mouse click press");
    if (report_input_event(fd) < 0)
        ERROR("write error: report event");
    mouse_ev.value = RELEASE_KEY;
    ret = write(fd, &mouse_ev, sizeof(struct input_event));
    if (ret < 0)
        ERROR("write error: mouse click release");
    if (report_input_event(fd) < 0)
        ERROR("write error: report event");
    return ret;
}

int move_cursor_rel(int fd, int rel_x, int rel_y)
{
    int ret;
    struct input_event mouse_ev;
    mouse_ev.type = EV_REL;
    mouse_ev.code = rel_x ? REL_X : REL_Y;
    mouse_ev.value = rel_x ? rel_x : rel_y;
    ret = write(fd, &mouse_ev, sizeof(struct input_event));
    if (ret < 0)
        ERROR("write error: mouse");
    if (report_input_event(fd) < 0)
        ERROR("write error: report event");
    return ret;
}

int move_cursor_abs(int fd, int abs_x, int abs_y)
{
    int ret;
    struct input_event mouse_ev;
    mouse_ev.type = EV_ABS;
    mouse_ev.code = ABS_X;
    mouse_ev.value = abs_x;
    ret = write(fd, &mouse_ev, sizeof(struct input_event));
    if (ret < 0)
        ERROR("write error: mouse");
    if (report_input_event(fd) < 0)
        ERROR("write error: report event");
    mouse_ev.code = ABS_Y;
    mouse_ev.value = abs_y;
    ret = write(fd, &mouse_ev, sizeof(struct input_event));
    if (ret < 0)
        ERROR("write error: mouse");
    if (report_input_event(fd) < 0)
        ERROR("write error: report event");
    mouse_ev.type = EV_KEY;
    mouse_ev.code = BTN_TOUCH;
    mouse_ev.value = 1;
    ret = write(fd, &mouse_ev, sizeof(struct input_event));
    if (ret < 0)
        ERROR("write error: mouse");
    if (report_input_event(fd) < 0)
        ERROR("write error: report event");
    mouse_ev.value = 0;
    ret = write(fd, &mouse_ev, sizeof(struct input_event));
    if (ret < 0)
        ERROR("write error: mouse");
    if (report_input_event(fd) < 0)
        ERROR("write error: report event");
    return ret;
}

int main(void)
{
    int fd_mouse;
    struct uinput_user_dev uidev;
    struct input_event ev;
    struct timeval timeval_t;
    char ch = 0;
    int mouse_sensitivity_default = 20;

    fd_mouse = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    if (fd_mouse < 0)
        ERROR("error: open mouse fd");

    if (ioctl(fd_mouse, UI_SET_EVBIT, EV_KEY) < 0)
        ERROR("error: ioctl: UI_SET_EVBIT");
    if (ioctl(fd_mouse, UI_SET_KEYBIT, BTN_LEFT) < 0)
        ERROR("error: ioctl: UI_SET_KEYBIT");
    if (ioctl(fd_mouse, UI_SET_KEYBIT, BTN_RIGHT) < 0)
        ERROR("error: ioctl: UI_SET_KEYBIT");
    if (ioctl(fd_mouse, UI_SET_EVBIT, EV_REL) < 0)
        ERROR("error: ioctl: UI_SET_EVBIT");
    if (ioctl(fd_mouse, UI_SET_RELBIT, REL_X) < 0)
        ERROR("error: ioctl: UI_SET_RELBIT");
    if (ioctl(fd_mouse, UI_SET_RELBIT, REL_Y) < 0)
        ERROR("error: ioctl: UI_SET_RELBIT");
    /* for touch input with x,y coordinate */
    if (ioctl(fd_mouse, UI_SET_EVBIT, EV_ABS) < 0)
        ERROR("error: ioctl: UI_SET_RELBIT");
    if (ioctl(fd_mouse, UI_SET_EVBIT, ABS_X) < 0)
        ERROR("error: ioctl: UI_SET_RELBIT");
    if (ioctl(fd_mouse, UI_SET_EVBIT, ABS_Y) < 0)
        ERROR("error: ioctl: UI_SET_RELBIT");        

    memset(&uidev, 0, sizeof(uidev));

    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "virtual mouse device");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1;
    uidev.id.product = 0x2;
    uidev.id.version = 1;

    if (write(fd_mouse, &uidev, sizeof(uidev)) < 0)
        ERROR("error: write");

    if (ioctl(fd_mouse, UI_DEV_CREATE) < 0)
        ERROR("error: ioctl");

    while (ch != 'q')
    {

        printf("\n\nselect operation: \
               \n w:cursor up \n d:cursor right \n a:cursor left \n z:cursor down \
               \n l:left click \n r:right click \n h: hold left mouse button pressed \n q:quit \n");

        scanf("%c", &ch);

        switch (ch)
        {
        case 'w':
        {
            move_cursor_rel(fd_mouse, 0, -(mouse_sensitivity_default));
            break;
        }
        case 'd':
        {
            move_cursor_rel(fd_mouse, mouse_sensitivity_default, 0);
            break;
        }
        case 'a':
        {
            move_cursor_rel(fd_mouse, -(mouse_sensitivity_default), 0);
            break;
        }
        case 'z':
        {
            move_cursor_rel(fd_mouse, 0, mouse_sensitivity_default);
            break;
        }
        case 'l':
        {
            click_mouse_button(fd_mouse, 1, 0);
            break;
        }
        case 'r':
        {
            click_mouse_button(fd_mouse, 0, 1);
            break;
        }
        case 'h':
        {
            hold_mouse_button_pressed(fd_mouse, 1, 0);
            break;
        }
        case 'q':
        {
            break;
        }
        default:
        {
            break;
        }
        }
    }

    sleep(1);

    if (ioctl(fd_mouse, UI_DEV_DESTROY) < 0)
        ERROR("error: ioctl");

    close(fd_mouse);

    return 0;
}
