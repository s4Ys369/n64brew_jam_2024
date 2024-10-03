 #include <libdrago.h>
 #include "core.h"

AiDiff global_core_aidifficulty = DIFF_MEDIUM;
float global_core_subtick = 0;

uint32_t core_get_playercount()
{
    return 0;
}

joypad_port_t core_get_playercontroller()
{
    return JOYPAD_PORT_1;
}

void core_set_subtick(float subtick)
{
    global_core_subtick = subtick;
}

AiDiff core_get_aidifficulty()
{
    return global_core_aidifficulty;
}

float core_get_subtick()
{
    return global_core_subtick;
}

void core_test_set_aidifficulty(AiDiff level)
{
    global_core_aidifficulty = level;
}