# ESP32-S3 Voice Wake Development Plan

## Project Goal

Build an ESP32-S3 based local voice wake system for industrial use.

The final system should:

- Listen through the I2S MEMS microphone.
- Detect a wake word locally.
- Recognize or map different command words.
- Send different signals to another device in the LAN through Wi-Fi.
- Keep standby power low when the system is only listening.

Current hardware and known working setup:

- Board: ESP32-S3 N16R8
- Microphone: I2S MEMS microphone
- Wiring:
  - `WS` -> `GPIO4`
  - `BCK/SCK` -> `GPIO5`
  - `SD/DOUT` -> `GPIO6`
- Audio channel: left channel
- Sample rate: `16000 Hz`
- Current verified feature: record 5 seconds of audio and save it as WAV on PC

## Development Rules

This project is also used as an embedded learning project, so the code should stay beginner friendly.

- Prefer simple and direct code.
- Keep functions short.
- Use clear file names and variable names.
- Add Chinese comments for important embedded concepts.
- Add one feature at a time.
- Verify each stage before moving to the next one.
- Avoid advanced C++ patterns unless there is a strong reason.

## Planned File Structure

```text
src/
  main.cpp
  config.h
  audio.h
  audio.cpp
  wifi_net.h
  wifi_net.cpp
  command.h
  command.cpp

tools/
  capture_wav.py

docs/
  development-plan.md
```

File responsibilities:

- `main.cpp`: program entry, state machine, main workflow.
- `config.h`: pins, sample rate, Wi-Fi settings, UDP target, thresholds.
- `audio.h/.cpp`: I2S setup, microphone reading, volume calculation.
- `wifi_net.h/.cpp`: Wi-Fi connection and UDP/TCP message sending.
- `command.h/.cpp`: command mapping, such as `start` -> `CMD_START`.
- `tools/capture_wav.py`: PC-side recording test tool.

## Stage 1: Split The Current Working Code

Goal:

Keep the current recording feature working, but split the code into modules.

Work items:

- Move I2S microphone initialization into `audio.cpp`.
- Move pin and sample rate settings into `config.h`.
- Keep `main.cpp` focused on calling setup and recording logic.
- Keep `tools/capture_wav.py` working.

Expected result:

- The board can still record 5 seconds of audio.
- The PC script can still generate a playable WAV file.

Verification:

```powershell
python .\tools\capture_wav.py --port COM3 --out test.wav
```

Pass condition:

- `test.wav` is generated.
- The recorded audio can be played normally.

Main risk:

- Splitting files may break include paths or global constants.

## Stage 2: Build Real-Time Listening Base

Goal:

Change from "record only when requested" to "continuously read microphone frames".

Work items:

- Add an audio frame reading function.
- Calculate a simple volume value from each frame.
- Print volume to Serial Plotter in the expected format.

Expected output format:

```text
>env:123
```

Expected result:

- The serial plotter shows a smooth volume curve.
- Speaking or clapping makes the curve rise.
- Silence keeps the curve low.

Verification:

- Open Serial Plotter at `115200`.
- Observe `env`.

Pass condition:

- `env` clearly changes between quiet and speaking.

Main risk:

- Industrial noise may keep `env` high, so later stages need adaptive thresholds.

## Stage 3: Add A Simple Wake State Machine

Goal:

Implement a basic wake behavior before adding Porcupine.

States:

- `IDLE_LISTEN`: keep listening.
- `ACTIVE_WINDOW`: wake has been triggered.

Simple trigger rule:

- If `env` is above a threshold for several continuous frames, enter `ACTIVE_WINDOW`.
- If no activity happens for a timeout period, return to `IDLE_LISTEN`.

Work items:

- Add state enum in `main.cpp`.
- Add threshold and timeout settings in `config.h`.
- Print state changes to serial.

Expected result:

- Loud sound or speech can trigger wake.
- Quiet environment will not trigger wake.
- The system returns to listening after timeout.

Verification:

- Clap or speak near the microphone.
- Check serial output for state changes.

Pass condition:

- State changes are stable and understandable.

Main risk:

- This is not real keyword detection. It is only a temporary test step.

## Stage 4: Add Wi-Fi LAN Message Sending

Goal:

After wake or command detection, send a signal to another device in the LAN.

Recommended first protocol:

- UDP

Reason:

- UDP is simple.
- It has low overhead.
- It is easy to test on a PC.

Work items:

- Add Wi-Fi SSID and password settings in `config.h`.
- Add target IP and port settings in `config.h`.
- Add `wifi_connect()` in `wifi_net.cpp`.
- Add `udp_send_message()` in `wifi_net.cpp`.
- Send a test message such as `WAKE_DETECTED`.

Expected result:

- ESP32-S3 connects to Wi-Fi.
- PC or another LAN device receives UDP messages.

Verification:

- Run a UDP listener on PC.
- Trigger wake on ESP32-S3.
- Confirm the PC receives the message.

Pass condition:

- LAN receiver gets the expected text message.

Main risks:

- Wrong Wi-Fi credentials.
- Wrong target IP.
- Firewall blocks UDP receiving on the PC.

## Stage 5: Add Command Mapping

Goal:

Support different commands and send different messages.

First implementation:

- Use serial input to simulate recognized commands.

Example mapping:

- `start` -> `CMD_START`
- `stop` -> `CMD_STOP`
- `estop` -> `CMD_ESTOP`
- `status` -> `CMD_STATUS`

Work items:

- Add `command.cpp`.
- Add a simple function that maps text command to LAN message.
- Test command messages through serial input first.

Expected result:

- Different command inputs produce different UDP messages.

Verification:

- Type `start`, `stop`, or `estop` in Serial Monitor.
- Check UDP receiver output.

Pass condition:

- The receiver gets the correct command message.

Main risk:

- This still does not include real speech recognition. It only validates the command path.

## Stage 6: Add Porcupine Wake Word Detection

Goal:

Replace simple volume wake with real local wake word detection.

Work items:

- Add Porcupine library and model files.
- Feed fixed-size audio frames into Porcupine.
- Trigger `WAKE_DETECTED` when Porcupine returns a positive result.
- Keep the existing state machine.

Expected result:

- The board wakes only when the configured wake word is spoken.
- Other sounds should not trigger wake frequently.

Verification:

- Test in quiet environment.
- Test with background machine noise.
- Test with different speakers.

Pass condition:

- The wake word is detected reliably.
- False wake rate is acceptable for the target environment.

Main risks:

- Porcupine integration may require specific frame size and sample format.
- ESP32-S3 memory usage must be checked.
- Wake word model and access key management must be handled correctly.

## Stage 7: Low Power And Industrial Tuning

Goal:

Make the system practical for long-term industrial use.

Work items:

- Keep Wi-Fi off during listening if possible.
- Turn Wi-Fi on only when sending messages or entering active mode.
- Add reconnect handling.
- Tune thresholds, timeouts, and Porcupine sensitivity.
- Reduce debug logs for deployment.
- Measure current consumption in each state.

Expected states:

- `LISTEN`: microphone and wake detector running.
- `ACTIVE`: command handling and Wi-Fi communication enabled.
- `COOLDOWN`: short delay before returning to listening.

Expected result:

- Lower average power usage.
- Stable wake behavior in the real work environment.

Verification:

- Measure current in each state.
- Run long tests in the target environment.
- Count false wakes and missed wakes.

Pass condition:

- Power, wake reliability, and command delivery meet project requirements.

Main risks:

- Wi-Fi reconnect time may be too slow for some use cases.
- Industrial noise may require better audio filtering.
- Very low power may conflict with continuous wake word detection.

## Milestone Summary

| Stage | Feature | Output |
| --- | --- | --- |
| 1 | Modularize current recorder | Recording still works |
| 2 | Real-time volume | Serial Plotter shows `env` |
| 3 | Simple wake state machine | Loud speech triggers active state |
| 4 | Wi-Fi UDP send | LAN device receives wake signal |
| 5 | Command mapping | Different commands send different messages |
| 6 | Porcupine wake word | Real keyword wake works |
| 7 | Low power tuning | System is closer to deployment |

## Next Step

Start with Stage 1.

The first coding task is to split the current working `src/main.cpp` into:

- `src/config.h`
- `src/audio.h`
- `src/audio.cpp`
- simplified `src/main.cpp`

The recording feature must still work after this change.
