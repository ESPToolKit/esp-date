#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <time.h>
#include <string>

struct DateTime {
  int64_t epochSeconds = 0;  // seconds since 1970-01-01T00:00:00Z

  int yearUtc() const;
  int monthUtc() const;   // 1..12
  int dayUtc() const;     // 1..31
  int hourUtc() const;    // 0..23
  int minuteUtc() const;  // 0..59
  int secondUtc() const;  // 0..59
};

enum class ESPDateFormat {
  Iso8601,
  DateTime,
  Date,
  Time
};

struct ESPDateConfig {
  float latitude = 0.0f;
  float longitude = 0.0f;
  const char* timeZone = nullptr;  // POSIX TZ string, e.g. "CET-1CEST,M3.5.0/2,M10.5.0/3"
  const char* ntpServer = nullptr; // optional NTP server; used with timeZone to call configTzTime
};

struct SunCycleResult {
  bool ok;
  DateTime value;
};

class ESPDate {
 public:
  ESPDate();
  explicit ESPDate(const ESPDateConfig& config);

  DateTime now() const;
  DateTime fromUnixSeconds(int64_t seconds) const;
  DateTime fromUtc(int year, int month, int day, int hour = 0, int minute = 0, int second = 0) const;
  DateTime fromLocal(int year, int month, int day, int hour = 0, int minute = 0, int second = 0) const;
  int64_t toUnixSeconds(const DateTime& dt) const;

  // Arithmetic relative to a provided DateTime
  DateTime addSeconds(const DateTime& dt, int64_t seconds) const;
  DateTime addMinutes(const DateTime& dt, int64_t minutes) const;
  DateTime addHours(const DateTime& dt, int64_t hours) const;
  DateTime addDays(const DateTime& dt, int32_t days) const;
  DateTime addMonths(const DateTime& dt, int32_t months) const;
  DateTime addYears(const DateTime& dt, int32_t years) const;

  DateTime subSeconds(const DateTime& dt, int64_t seconds) const;
  DateTime subMinutes(const DateTime& dt, int64_t minutes) const;
  DateTime subHours(const DateTime& dt, int64_t hours) const;
  DateTime subDays(const DateTime& dt, int32_t days) const;
  DateTime subMonths(const DateTime& dt, int32_t months) const;
  DateTime subYears(const DateTime& dt, int32_t years) const;

  // Convenience arithmetic relative to now()
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

  // Differences
  int64_t differenceInSeconds(const DateTime& a, const DateTime& b) const;
  int64_t differenceInMinutes(const DateTime& a, const DateTime& b) const;
  int64_t differenceInHours(const DateTime& a, const DateTime& b) const;
  int64_t differenceInDays(const DateTime& a, const DateTime& b) const;

  // Comparisons
  bool isBefore(const DateTime& a, const DateTime& b) const;
  bool isAfter(const DateTime& a, const DateTime& b) const;
  bool isEqual(const DateTime& a, const DateTime& b) const;           // seconds precision
  bool isEqualMinutes(const DateTime& a, const DateTime& b) const;    // minutes precision (UTC epoch / 60)
  bool isEqualMinutesUtc(const DateTime& a, const DateTime& b) const; // alias for minute-level UTC compare
  bool isSameDay(const DateTime& a, const DateTime& b) const;

  // Calendar helpers (UTC)
  DateTime startOfDayUtc(const DateTime& dt) const;
  DateTime endOfDayUtc(const DateTime& dt) const;
  DateTime startOfMonthUtc(const DateTime& dt) const;
  DateTime endOfMonthUtc(const DateTime& dt) const;

  int getYearUtc(const DateTime& dt) const;
  int getMonthUtc(const DateTime& dt) const;   // 1..12
  int getDayUtc(const DateTime& dt) const;     // 1..31
  int getWeekdayUtc(const DateTime& dt) const; // 0=Sunday..6=Saturday

  // Local time helpers (respect TZ)
  DateTime startOfDayLocal(const DateTime& dt) const;
  DateTime endOfDayLocal(const DateTime& dt) const;
  DateTime startOfMonthLocal(const DateTime& dt) const;
  DateTime endOfMonthLocal(const DateTime& dt) const;
  DateTime startOfYearUtc(const DateTime& dt) const;
  DateTime startOfYearLocal(const DateTime& dt) const;

  DateTime setTimeOfDayLocal(const DateTime& dt, int hour, int minute, int second) const;
  DateTime setTimeOfDayUtc(const DateTime& dt, int hour, int minute, int second) const;
  DateTime nextDailyAtLocal(int hour, int minute, int second, const DateTime& from) const;
  DateTime nextWeekdayAtLocal(int weekday, int hour, int minute, int second, const DateTime& from) const;

  int getYearLocal(const DateTime& dt) const;
  int getMonthLocal(const DateTime& dt) const;   // 1..12
  int getDayLocal(const DateTime& dt) const;     // 1..31
  int getWeekdayLocal(const DateTime& dt) const; // 0=Sunday..6=Saturday

  bool isLeapYear(int year) const;
  int daysInMonth(int year, int month) const; // month: 1..12

  // Formatting
  bool formatUtc(const DateTime& dt, ESPDateFormat style, char* outBuffer, size_t outSize) const;
  bool formatLocal(const DateTime& dt, ESPDateFormat style, char* outBuffer, size_t outSize) const;
  bool formatWithPatternUtc(const DateTime& dt, const char* pattern, char* outBuffer, size_t outSize) const;
  bool formatWithPatternLocal(const DateTime& dt, const char* pattern, char* outBuffer, size_t outSize) const;

  struct ParseResult {
    bool ok;
    DateTime value;
  };

  ParseResult parseIso8601Utc(const char* str) const;
  ParseResult parseDateTimeLocal(const char* str) const;

  // Sun cycle using stored configuration (lat/lon/timezone)
  SunCycleResult sunrise() const;
  SunCycleResult sunset() const;
  SunCycleResult sunrise(const DateTime& day) const;
  SunCycleResult sunset(const DateTime& day) const;

  // Sun cycle with explicit parameters (timezone in hours, DST flag)
  SunCycleResult sunrise(float latitude, float longitude, float timezoneHours, bool isDst) const;
  SunCycleResult sunset(float latitude, float longitude, float timezoneHours, bool isDst) const;
  SunCycleResult sunrise(float latitude,
                         float longitude,
                         float timezoneHours,
                         bool isDst,
                         const DateTime& day) const;
  SunCycleResult sunset(float latitude,
                        float longitude,
                        float timezoneHours,
                        bool isDst,
                        const DateTime& day) const;

  // Sun cycle using a POSIX TZ string (auto-DST) instead of numeric offset
  SunCycleResult sunrise(float latitude, float longitude, const char* timeZone) const;
  SunCycleResult sunset(float latitude, float longitude, const char* timeZone) const;
  SunCycleResult sunrise(float latitude, float longitude, const char* timeZone, const DateTime& day) const;
  SunCycleResult sunset(float latitude, float longitude, const char* timeZone, const DateTime& day) const;

  // Daylight checks using stored configuration
  bool isDay() const;
  bool isDay(const DateTime& day) const;
  bool isDay(int sunRiseOffsetSec, int sunSetOffsetSec) const;
  bool isDay(int sunRiseOffsetSec, int sunSetOffsetSec, const DateTime& day) const;

  // Daylight saving time helpers
  bool isDstActive() const;
  bool isDstActive(const DateTime& dt) const;
  bool isDstActive(const char* timeZone) const;
  bool isDstActive(const DateTime& dt, const char* timeZone) const;

  // Month names
  const char* monthName(int month) const;         // 1..12, returns "January" ..."December" or nullptr on invalid
  const char* monthName(const DateTime& dt) const;

 private:
  SunCycleResult sunriseFromConfig(const DateTime& day) const;
  SunCycleResult sunsetFromConfig(const DateTime& day) const;
  bool isDayWithOffsets(const DateTime& day, int sunRiseOffsetSec, int sunSetOffsetSec) const;

  float latitude_ = 0.0f;
  float longitude_ = 0.0f;
  std::string timeZone_;
  bool hasLocation_ = false;
};
