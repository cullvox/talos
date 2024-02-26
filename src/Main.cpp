#include "App.h"

/* Talos uses a large stack size, Spotify and other libraries can 
    use lots of it. However this is probably too much stack to be
    using up lol. */
SET_LOOP_TASK_STACK_SIZE(1024*16)

ts::Talos app;

void setup() {
    app.init();
}

void loop() {
    app.run();
}
