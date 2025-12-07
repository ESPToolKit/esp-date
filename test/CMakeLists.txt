# ESPDate relies on Arduino/ESP-IDF time and Arduino.h, so host-side CMake
# builds do not provide a usable environment for the Unity tests. Run the
# tests on-device via PlatformIO/Arduino instead.
message(STATUS "ESPDate: Unity tests are Arduino/PlatformIO only; skipping host build.")
