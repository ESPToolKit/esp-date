# ESPDate

ESPDate is a tiny C++17 helper for ESP32 projects that makes working with dates and times feel more like using `date-fns` in JavaScript/TypeScript. It wraps `time_t` / `struct tm` and adds safe arithmetic, comparisons, and formatting in a single class-based API.

## CI / Release / License
[![CI](https://github.com/ESPToolKit/esp-date/actions/workflows/ci.yml/badge.svg)](https://github.com/ESPToolKit/esp-date/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/ESPToolKit/esp-date?sort=semver)](https://github.com/ESPToolKit/esp-date/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE.md)

## Features
- **DateTime wrapper**: one small `DateTime` value type instead of juggling raw `time_t` + `struct tm`.
- **Safe arithmetic helpers**: `add/subSeconds`, `add/subMinutes`, `add/subHours`, `add/subDays`, `add/subMonths`, `add/subYears`.
- **Differences & comparisons**: `differenceIn*`, `isBefore`, `isAfter`, `isEqual`, `isSameDay`.
- **Calendar helpers**: `startOfDay*`, `endOfDay*`, `startOfMonth*`, `endOfMonth*`, `isLeapYear`, `daysInMonth`, getters for year/month/day/weekday.
- **Formatting / parsing**: ISO-8601 and `YYYY-MM-DD HH:MM:SS` helpers, plus `strftime`-style patterns for UTC or local time.
- **Class-based API**: everything hangs off a single `ESPDate` instance; no global namespace clutter.
- **Lightweight & portable**: C++17, header-first public API; relies only on standard C time functions and the system clock (`time()`).

ESPDate does **not** configure SNTP or time zones. You remain in control of how system time is synchronised; ESPDate only makes it easier to work with once it is set.

## Getting Started
Install one of two ways:
- Download the repository zip from GitHub, extract it, and drop the folder into your PlatformIO `lib/` directory, Arduino IDE `libraries/` directory, or add it as an ESP-IDF component.
- Add the public GitHub URL to `lib_deps` in `platformio.ini` so PlatformIO fetches it for you:
  ```
  lib_deps = https://github.com/ESPToolKit/esp-date.git
  ```

Then include the umbrella header:

```cpp
#include <Arduino.h>
#include <ESPDate.h>

static ESPDate date;  // module-style instance

void setup() {
    Serial.begin(115200);

    // Make sure system time is set up (SNTP / manual) before calling date.now()

    DateTime now = date.now();             // current time from system clock
    DateTime lastYear = date.subYears(1);  // 1 year before now

    int64_t diffSeconds = date.differenceInSeconds(now, lastYear);
    int64_t diffDays    = date.differenceInDays(now, lastYear);

    bool isBefore = date.isBefore(lastYear, now); // true

    char buf[32];
    if (date.formatUtc(now, ESPDateFormat::Iso8601, buf, sizeof(buf))) {
        Serial.print("Now (UTC): ");
        Serial.println(buf);
    }

    Serial.print("Seconds between now and last year: ");
    Serial.println(diffSeconds);
    Serial.print("Days between now and last year: ");
    Serial.println(diffDays);
}

void loop() {
    // ...
}
```

## Date & Time Model
`DateTime` is a small value type representing a moment in time, backed by seconds since the Unix epoch:

```cpp
struct DateTime {
    int64_t epochSeconds;   // seconds since 1970-01-01T00:00:00Z

    int yearUtc() const;
    int monthUtc() const;   // 1..12
    int dayUtc() const;     // 1..31
    int hourUtc() const;    // 0..23
    int minuteUtc() const;  // 0..59
    int secondUtc() const;  // 0..59
};
```

It is cheap to copy (just an `int64_t`), safe to compare and subtract, and convertible to/from `struct tm` internally by ESPDate. You never manipulate `struct tm` directly—always go through ESPDate.

## API Overview
The main module-type class you will use:

```cpp
class ESPDate {
public:
    // Time sources
    DateTime now() const;
    DateTime fromUnixSeconds(int64_t seconds) const;
    DateTime fromUtc(int year, int month, int day, int hour = 0, int minute = 0, int second = 0) const;
    DateTime fromLocal(int year, int month, int day, int hour = 0, int minute = 0, int second = 0) const;
    int64_t toUnixSeconds(const DateTime &dt) const;

    // Arithmetic (UTC-backed)
    DateTime addSeconds(const DateTime &dt, int64_t seconds) const;
    DateTime addMinutes(const DateTime &dt, int64_t minutes) const;
    DateTime addHours(const DateTime &dt, int64_t hours) const;
    DateTime addDays(const DateTime &dt, int32_t days) const;
    DateTime addMonths(const DateTime &dt, int32_t months) const;
    DateTime addYears(const DateTime &dt, int32_t years) const;

    DateTime subSeconds(const DateTime &dt, int64_t seconds) const;
    DateTime subMinutes(const DateTime &dt, int64_t minutes) const;
    DateTime subHours(const DateTime &dt, int64_t hours) const;
    DateTime subDays(const DateTime &dt, int32_t days) const;
    DateTime subMonths(const DateTime &dt, int32_t months) const;
    DateTime subYears(const DateTime &dt, int32_t years) const;

    // Convenience: relative to now()
    DateTime addSeconds(int64_t seconds) const;
    DateTime addMinutes(int64_t minutes) const;
    DateTime addHours(int64_t hours) const;
    DateTime addDays(int32_t days) const;
    DateTime addMonths(int32_t months) const;
    DateTime addYears(int32_t years) const;

    DateTime subSeconds(int64_t seconds) const;
    DateTime subMinutes(int64_t minutes) const;
    DateTime subHours(int64_t hours) const;
    DateTime subDays(int32_t days) const;
    DateTime subMonths(int32_t months) const;
    DateTime subYears(int32_t years) const;

    // Differences & comparisons
    int64_t differenceInSeconds(const DateTime &a, const DateTime &b) const;
    int64_t differenceInMinutes(const DateTime &a, const DateTime &b) const;
    int64_t differenceInHours(const DateTime &a, const DateTime &b) const;
    int64_t differenceInDays(const DateTime &a, const DateTime &b) const;
    bool isBefore(const DateTime &a, const DateTime &b) const;
    bool isAfter(const DateTime &a, const DateTime &b) const;
    bool isEqual(const DateTime &a, const DateTime &b) const;
    bool isSameDay(const DateTime &a, const DateTime &b) const; // UTC calendar day

    // Calendar helpers (UTC and local variants)
    DateTime startOfDayUtc(const DateTime &dt) const;
    DateTime endOfDayUtc(const DateTime &dt) const;
    DateTime startOfMonthUtc(const DateTime &dt) const;
    DateTime endOfMonthUtc(const DateTime &dt) const;
    DateTime startOfDayLocal(const DateTime &dt) const;
    DateTime endOfDayLocal(const DateTime &dt) const;
    DateTime startOfMonthLocal(const DateTime &dt) const;
    DateTime endOfMonthLocal(const DateTime &dt) const;
    DateTime startOfYearUtc(const DateTime &dt) const;
    DateTime startOfYearLocal(const DateTime &dt) const;
    DateTime setTimeOfDayLocal(const DateTime &dt, int hour, int minute, int second) const;
    DateTime setTimeOfDayUtc(const DateTime &dt, int hour, int minute, int second) const;
    DateTime nextDailyAtLocal(int hour, int minute, int second, const DateTime &from) const;
    DateTime nextWeekdayAtLocal(int weekday, int hour, int minute, int second, const DateTime &from) const;

    int getYearUtc(const DateTime &dt) const;
    int getMonthUtc(const DateTime &dt) const;   // 1..12
    int getDayUtc(const DateTime &dt) const;     // 1..31
    int getWeekdayUtc(const DateTime &dt) const; // 0=Sun..6=Sat
    int getYearLocal(const DateTime &dt) const;
    int getMonthLocal(const DateTime &dt) const;
    int getDayLocal(const DateTime &dt) const;
    int getWeekdayLocal(const DateTime &dt) const;

    bool isLeapYear(int year) const;
    int  daysInMonth(int year, int month) const; // month: 1..12

    // Formatting
    bool formatUtc(const DateTime &dt, ESPDateFormat style, char *outBuffer, size_t outSize) const;
    bool formatLocal(const DateTime &dt, ESPDateFormat style, char *outBuffer, size_t outSize) const;
    bool formatWithPatternUtc(const DateTime &dt, const char *pattern, char *outBuffer, size_t outSize) const;
    bool formatWithPatternLocal(const DateTime &dt, const char *pattern, char *outBuffer, size_t outSize) const;

    struct ParseResult { bool ok; DateTime value; };
    ParseResult parseIso8601Utc(const char *str) const;           // "YYYY-MM-DDTHH:MM:SSZ"
    ParseResult parseDateTimeLocal(const char *str) const;        // "YYYY-MM-DD HH:MM:SS"
};
```

## Examples
Difference between timestamps:

```cpp
DateTime now = date.now();
DateTime yesterday = date.subDays(1);

int64_t sec = date.differenceInSeconds(now, yesterday);
int64_t min = date.differenceInMinutes(now, yesterday);
int64_t days = date.differenceInDays(now, yesterday);

Serial.printf("Δ: %lld s, %lld min, %lld days\n",
              static_cast<long long>(sec),
              static_cast<long long>(min),
              static_cast<long long>(days));
```

Start/end of day (local):

```cpp
DateTime now = date.now();
DateTime start = date.startOfDayLocal(now);
DateTime end   = date.endOfDayLocal(now);

char buf[32];
date.formatLocal(start, ESPDateFormat::DateTime, buf, sizeof(buf));
Serial.print("Day starts at: ");
Serial.println(buf);

date.formatLocal(end, ESPDateFormat::DateTime, buf, sizeof(buf));
Serial.print("Day ends at: ");
Serial.println(buf);
```

Calculating the next month’s billing date:

```cpp
DateTime now = date.now();
DateTime thisBilling = date.setTimeOfDayLocal(
    date.startOfMonthLocal(now),
    3, 0, 0); // 03:00 local on the 1st

DateTime nextBilling = date.addMonths(thisBilling, 1);
```

## Gotchas
- ESPDate never configures SNTP or timezone rules; ensure the device clock is set before calling `now()`.
- All arithmetic and comparisons are UTC-first. Local helpers rely on the current process TZ (`setenv("TZ", ...)`, `tzset()`); make sure that matches your deployment.
- Month/year arithmetic clamps to the last valid day of the target month (e.g., Jan 31 + 1 month → Feb 28/29; Feb 29 - 1 year → Feb 28).
- `differenceInDays` is purely `seconds / 86400` truncated toward zero, not a calendar-boundary delta.
- Leap seconds are treated like 60th seconds in parsing; they are not modeled beyond that.
- `isSameDay` compares the UTC calendar day. Use `startOfDayLocal` / `endOfDayLocal` if you need local-day comparisons.
- The library avoids dynamic allocations and exceptions; formatting returns `false` if buffers are too small or time conversion fails.
- ESP32 toolchains ship a 64-bit `time_t`; ESPDate assumes that width (there is a `static_assert` to catch 32-bit time_t).

## Restrictions
- ESP32 + FreeRTOS (Arduino-ESP32 or ESP-IDF) with C++17 enabled.
- Requires a working system clock (`time()`) and relies on POSIX-style TZ handling for local-time helpers.

## Tests
- CI builds examples via PlatformIO and Arduino CLI on common ESP32 boards to ensure the API compiles cleanly under ArduinoJson-installed environments.
- When using Arduino CLI locally, mirror CI by priming the ESP32 board manager URL before installing the core:
  ```bash
  arduino-cli config init --overwrite
  arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  arduino-cli core update-index --additional-urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  arduino-cli core install esp32:esp32@3.3.3 --additional-urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```
- You can also run `pio ci examples/basic_date --board esp32dev --project-option "build_flags=-std=gnu++17"` locally.
- Unity smoke tests live in `test/test_esp_date`; run them on hardware with `pio test -e esp32dev` (or your board environment) to exercise arithmetic, formatting, and parsing routines.

## License
MIT — see [LICENSE.md](LICENSE.md).

## ESPToolKit
- Check out other libraries: <https://github.com/orgs/ESPToolKit/repositories>
- Support the project: <https://ko-fi.com/esptoolkit>
- Visit the website: <https://www.esptoolkit.hu/>
