// Env-selectable entrypoint that pulls in the appropriate example and variant glue.

#if defined(EXAMPLE_REPEATER)
#include "../examples/simple_repeater/main.cpp"
#elif defined(EXAMPLE_COMPANION_BLE)
#include "../examples/companion_radio/main.cpp"
#elif defined(EXAMPLE_ROOM_SERVER)
#include "../examples/simple_room_server/main.cpp"
#else
#error "No example selected. Define one of: EXAMPLE_REPEATER, EXAMPLE_COMPANION_BLE, EXAMPLE_ROOM_SERVER"
#endif

// Variant hardware glue is compiled separately via build_src_filter.
