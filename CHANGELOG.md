# Changelog

All notable changes to this project are documented in this file.

The format follows Keep a Changelog and the project adheres to Semantic Versioning.

## [Unreleased]
### Added
- Unity smoke tests under `test/test_esp_date` to cover arithmetic, formatting, and parsing flows on-device.
- Documented both installation paths (manual zip drop-in and PlatformIO `lib_deps` GitHub URL) in the README Getting Started section.
- Convenience constructors (`fromUtc`, `fromLocal`), start-of-year helpers, and scheduler-friendly helpers (`nextDailyAtLocal`, `nextWeekdayAtLocal`) to simplify consumer code.

### Fixed
- Seeded the CI Arduino CLI setup with the ESP32 board manager URL so core installs succeed on clean runners.
- Clarified date arithmetic/difference semantics in README and downgraded the `time_t` width check to a warning for 32-bit toolchains.

## [0.1.0] - 2025-12-07
### Added
- Introduced the `DateTime` value type backed by epoch seconds with UTC calendar accessors.
- Added arithmetic helpers for seconds, minutes, hours, days, months, and years, plus calendar boundaries (`startOfDay*`, `endOfMonth*`) in UTC and local time.
- Implemented formatting helpers (ISO-8601, datetime, date, time, and custom patterns) and parsing for ISO-8601 UTC or local `YYYY-MM-DD HH:MM:SS`.
- Included comparison/difference helpers (`isBefore`, `isAfter`, `isEqual`, `isSameDay`, `differenceIn*`) and convenience setters like `setTimeOfDayLocal`.
- Added a basic Arduino sketch demonstrating common flows.

### Tooling
- Added Arduino and PlatformIO metadata, CMake scaffolding, CI/release workflows, and repo hygiene files (code of conduct, license, changelog).

### Documentation
- Authored the README describing goals, feature overview, examples, gotchas, and ESPToolKit links.

[Unreleased]: https://github.com/ESPToolKit/esp-date/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/ESPToolKit/esp-date/releases/tag/v0.1.0
