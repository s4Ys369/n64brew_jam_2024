 #include <libdrago.h>
 #include "core.h"

AiDiff global_core_aidifficulty = DIFF_MEDIUM;

uint32_t core_get_playercount()
{
    return 0;
}

joypad_port_t core_get_playercontroller()
{
    return JOYPAD_PORT_1;
}

AiDiff core_get_aidifficulty()
{
    return global_core_aidifficulty;
}

void core_set_winner(PlyNum ply)
{

}

void core_test_set_aidifficulty(AiDiff level)
{
    global_core_aidifficulty = level;
}