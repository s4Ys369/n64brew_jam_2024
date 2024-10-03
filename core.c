 #include <libdragon.h>
 #include "core.h"

#define FINAL 0

AiDiff global_core_aidifficulty = DIFF_MEDIUM;
double global_core_subtick = 0;


uint32_t core_get_playercount()
{
    // TODO
    return 0;
}

joypad_port_t core_get_playercontroller()
{
    // TODO
    return JOYPAD_PORT_1;
}

void core_set_subtick(double subtick)
{
    global_core_subtick = subtick;
}

void core_set_winner(PlyNum ply)
{
    // TODO
}

AiDiff core_get_aidifficulty()
{
    return global_core_aidifficulty;
}

double core_get_subtick()
{
    return global_core_subtick;
}

void core_test_set_aidifficulty(AiDiff level)
{
    #if FINAL
        (void)level;
    #else
        global_core_aidifficulty = level;
    #endif
}