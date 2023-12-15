#include "App.h"

ts::App app;

SET_LOOP_TASK_STACK_SIZE(12 * 1024);

void setup()
{
    app.init();
}

void loop() 
{
}
