#ifndef CONTROLS_H
#define CONTROLS_H


typedef struct {

    joypad_buttons_t pressed;
    joypad_buttons_t held;
    joypad_buttons_t released;
    joypad_inputs_t input;

} ControllerData;


void controllerData_getInputs(ControllerData*data, uint8_t port);

void controllerData_getInputs(ControllerData* data, uint8_t port)
{
    data->pressed = joypad_get_buttons_pressed(port);
    data->held = joypad_get_buttons_held(port);
    data->released = joypad_get_buttons_released(port);
    data->input = joypad_get_inputs(port); 
}

#define DEAD_ZONE 50
#define INPUT_DELAY 0.2f

// Treats joystick inputs as digital pad buttons, for menu navigation
void controllerData_8way(ControllerData* data) {
    static float input_time = 0;
    const float current_time = display_get_delta_time();

    input_time -= current_time;

    if(input_time <= 0.0f)
    {
        if (data->input.stick_y > DEAD_ZONE) {
            data->pressed.d_up = 1;
            input_time = INPUT_DELAY;
        } else if (data->input.stick_y < -DEAD_ZONE) {
            data->pressed.d_down = 1;
            input_time = INPUT_DELAY;
        }

        if (data->input.stick_x > DEAD_ZONE) {
            data->pressed.d_right = 1;
            input_time = INPUT_DELAY;
        } else if (data->input.stick_x < -DEAD_ZONE) {
            data->pressed.d_left = 1;
            input_time = INPUT_DELAY;
        }
    }

}

#endif