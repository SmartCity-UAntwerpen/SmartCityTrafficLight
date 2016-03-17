#include "robotapp.h"
#include <libpiface-1.0/pfio.h>

void RobotApp(int argc, char *argv[])
{
    pfio_init();

    while (1)
    {
        pfio_digital_write(LIGHT1, RED);
        pfio_digital_write(LIGHT2, RED);
        _delay_ms(SWITCH_TIME);
        pfio_digital_write(LIGHT1, RED);
        pfio_digital_write(LIGHT2, GREEN);
        _delay_ms(CYCLE_TIME);
        pfio_digital_write(LIGHT1, RED);
        pfio_digital_write(LIGHT2, RED);
        _delay_ms(SWITCH_TIME);
        pfio_digital_write(LIGHT1, GREEN);
        pfio_digital_write(LIGHT2, RED);
        _delay_ms(CYCLE_TIME);
    }

    pfio_deinit();
    printf ("Ready.\n");
}
