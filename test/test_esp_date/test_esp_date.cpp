#include <Arduino.h>
#include <ESPDate.h>
#include <unity.h>
#include <time.h>
#include <cstdlib>
#include <string>

static ESPDate date;
static const float kBudapestLat = 47.4979f;
static const float kBudapestLon = 19.0402f;

static void test_add_days_and_differences() {
  DateTime base = date.fromUnixSeconds(1704067200);  // 2024-01-01T00:00:00Z
  DateTime plus = date.addDays(base, 1);
  DateTime minus = date.subDays(base, 1);

  TEST_ASSERT_EQUAL(2024, date.getYearUtc(plus));
  TEST_ASSERT_EQUAL(1, date.getMonthUtc(plus));
  TEST_ASSERT_EQUAL(2, date.getDayUtc(plus));

  TEST_ASSERT_EQUAL(2023, date.getYearUtc(minus));
  TEST_ASSERT_EQUAL(12, date.getMonthUtc(minus));
  TEST_ASSERT_EQUAL(31, date.getDayUtc(minus));

  TEST_ASSERT_EQUAL_INT(1, date.differenceInDays(plus, base));
  TEST_ASSERT_EQUAL_INT(-1, date.differenceInDays(minus, base));
  TEST_ASSERT_TRUE(date.isAfter(plus, base));
  TEST_ASSERT_TRUE(date.isBefore(minus, base));
}

static void test_add_months_clamps_day_in_leap_year() {
  DateTime jan31 = date.fromUnixSeconds(1706659200);  // 2024-01-31T00:00:00Z
  DateTime feb = date.addMonths(jan31, 1);

  TEST_ASSERT_EQUAL(2024, date.getYearUtc(feb));
  TEST_ASSERT_EQUAL(2, date.getMonthUtc(feb));
  TEST_ASSERT_EQUAL(29, date.getDayUtc(feb));  // clamps to Feb 29 on leap year
}

static void test_start_and_end_of_day_utc() {
  DateTime midday = date.fromUnixSeconds(1709652610);  // 2024-03-05T15:30:10Z
  DateTime start = date.startOfDayUtc(midday);
  DateTime end = date.endOfDayUtc(midday);

  TEST_ASSERT_EQUAL(0, start.hourUtc());
  TEST_ASSERT_EQUAL(0, start.minuteUtc());
  TEST_ASSERT_EQUAL(0, start.secondUtc());

  TEST_ASSERT_EQUAL(23, end.hourUtc());
  TEST_ASSERT_EQUAL(59, end.minuteUtc());
  TEST_ASSERT_EQUAL(59, end.secondUtc());
  TEST_ASSERT_TRUE(date.isSameDay(midday, start));
  TEST_ASSERT_TRUE(date.isSameDay(midday, end));
}

static void test_parse_and_format_iso_utc() {
  ESPDate::ParseResult parsed = date.parseIso8601Utc("2025-01-01T00:00:00Z");
  TEST_ASSERT_TRUE(parsed.ok);
  TEST_ASSERT_TRUE(date.isEqual(parsed.value, date.fromUnixSeconds(1735689600)));

  char buf[32];
  DateTime moment = date.fromUnixSeconds(1767225570);  // 2025-12-31T23:59:30Z
  TEST_ASSERT_TRUE(date.formatUtc(moment, ESPDateFormat::Iso8601, buf, sizeof(buf)));
  TEST_ASSERT_EQUAL_STRING("2025-12-31T23:59:30Z", buf);
}

static void test_from_utc_clamps_day() {
  DateTime dt = date.fromUtc(2025, 2, 30);
  TEST_ASSERT_EQUAL(2025, date.getYearUtc(dt));
  TEST_ASSERT_EQUAL(2, date.getMonthUtc(dt));
  TEST_ASSERT_EQUAL(28, date.getDayUtc(dt));
  TEST_ASSERT_TRUE(date.isEqual(dt, date.fromUnixSeconds(1740700800)));  // 2025-02-28T00:00:00Z
}

static void test_start_of_year_helpers() {
  DateTime mid = date.fromUnixSeconds(1709652610);  // 2024-03-05T15:30:10Z
  DateTime startUtc = date.startOfYearUtc(mid);
  DateTime startLocal = date.startOfYearLocal(mid);
  DateTime expected = date.fromUnixSeconds(1704067200);  // 2024-01-01T00:00:00Z
  TEST_ASSERT_TRUE(date.isEqual(startUtc, expected));
  TEST_ASSERT_TRUE(date.isEqual(startLocal, expected));
}

static void test_next_daily_and_weekday_local() {
  DateTime before = date.fromUnixSeconds(1741157940);  // 2025-03-05T06:59:00Z
  DateTime atEight = date.nextDailyAtLocal(8, 0, 0, before);
  TEST_ASSERT_TRUE(date.isEqual(atEight, date.fromUnixSeconds(1741161600)));  // same day at 08:00

  DateTime monday930 = date.nextWeekdayAtLocal(1, 9, 30, 0, before);  // 1 = Monday
  TEST_ASSERT_TRUE(date.isEqual(monday930, date.fromUnixSeconds(1741599000)));  // 2025-03-10T09:30:00Z
}

static void test_sunrise_config_matches_manual() {
  ESPDate configured;
  configured.init(ESPDateConfig{kBudapestLat, kBudapestLon, "CET-1CEST,M3.5.0/2,M10.5.0/3"});
  DateTime day = configured.fromUtc(2024, 6, 1);

  SunCycleResult cfgRise = configured.sunrise(day);
  SunCycleResult cfgSet = configured.sunset(day);
  SunCycleResult manualRise = date.sunrise(kBudapestLat, kBudapestLon, 1.0f, true, day);   // CEST
  SunCycleResult manualSet = date.sunset(kBudapestLat, kBudapestLon, 1.0f, true, day);

  TEST_ASSERT_TRUE(cfgRise.ok);
  TEST_ASSERT_TRUE(cfgSet.ok);
  TEST_ASSERT_TRUE(manualRise.ok);
  TEST_ASSERT_TRUE(manualSet.ok);

  int64_t riseDelta = date.differenceInMinutes(cfgRise.value, manualRise.value);
  int64_t setDelta = date.differenceInMinutes(cfgSet.value, manualSet.value);

  TEST_ASSERT_TRUE(llabs(riseDelta) <= 2);
  TEST_ASSERT_TRUE(llabs(setDelta) <= 2);

  setenv("TZ", "UTC", 1);
  tzset();
}

static void test_is_day_helpers() {
  ESPDate solar;
  solar.init(ESPDateConfig{kBudapestLat, kBudapestLon, "CET-1CEST,M3.5.0/2,M10.5.0/3"});
  DateTime day = solar.fromUtc(2024, 6, 1);
  SunCycleResult rise = solar.sunrise(day);
  SunCycleResult set = solar.sunset(day);
  TEST_ASSERT_TRUE(rise.ok);
  TEST_ASSERT_TRUE(set.ok);

  DateTime morning = solar.addMinutes(rise.value, 30);
  DateTime night = solar.subMinutes(rise.value, 30);
  TEST_ASSERT_TRUE(solar.isDay(morning));
  TEST_ASSERT_FALSE(solar.isDay(night));

  int sunriseOffset = -900;  // 15 minutes before sunrise counts as day
  DateTime preDawn = solar.subMinutes(rise.value, 10);
  TEST_ASSERT_TRUE(solar.isDay(sunriseOffset, 0, preDawn));
  TEST_ASSERT_FALSE(solar.isDay(0, 0, preDawn));

  // Large negative sunset offset should end the day earlier
  int sunsetOffset = -3600;  // end one hour earlier
  DateTime beforeEarlyEnd = solar.subMinutes(set.value, 30);
  TEST_ASSERT_TRUE(solar.isDay(0, sunsetOffset, beforeEarlyEnd));
  DateTime afterEarlyEnd = solar.subMinutes(set.value, -10);
  TEST_ASSERT_FALSE(solar.isDay(0, sunsetOffset, afterEarlyEnd));

  setenv("TZ", "UTC", 1);
  tzset();
}

static void test_is_dst_active_with_timezone_string() {
  DateTime summer = date.fromUtc(2024, 6, 1, 12, 0, 0);
  DateTime winter = date.fromUtc(2024, 12, 1, 12, 0, 0);

  TEST_ASSERT_TRUE(date.isDstActive(summer, "CET-1CEST,M3.5.0/2,M10.5.0/3"));
  TEST_ASSERT_FALSE(date.isDstActive(winter, "CET-1CEST,M3.5.0/2,M10.5.0/3"));
}

static void test_is_dst_active_with_configured_timezone() {
  ESPDate configured;
  configured.init(ESPDateConfig{0.0f, 0.0f, "EST5EDT,M3.2.0/2,M11.1.0/2"});
  DateTime summer = configured.fromUtc(2024, 7, 1, 15, 0, 0);
  DateTime winter = configured.fromUtc(2024, 12, 1, 15, 0, 0);

  TEST_ASSERT_TRUE(configured.isDstActive(summer));
  TEST_ASSERT_FALSE(configured.isDstActive(winter));
}

static void test_is_dst_active_with_system_timezone() {
  const char* current = getenv("TZ");
  std::string previous = current ? current : "";
  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();

  DateTime summer = date.fromUtc(2024, 6, 1, 12, 0, 0);
  DateTime winter = date.fromUtc(2024, 1, 15, 12, 0, 0);

  TEST_ASSERT_TRUE(date.isDstActive(summer));
  TEST_ASSERT_FALSE(date.isDstActive(winter));

  if (previous.empty()) {
    unsetenv("TZ");
  } else {
    setenv("TZ", previous.c_str(), 1);
  }
  tzset();
}

static void test_to_local_breakdown() {
  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();

  DateTime winter = date.fromUtc(2024, 12, 1, 20, 45, 0);  // 21:45 CET
  LocalDateTime winterLocal = date.toLocal(winter);
  TEST_ASSERT_TRUE(winterLocal.ok);
  TEST_ASSERT_EQUAL(2024, winterLocal.year);
  TEST_ASSERT_EQUAL(12, winterLocal.month);
  TEST_ASSERT_EQUAL(1, winterLocal.day);
  TEST_ASSERT_EQUAL(21, winterLocal.hour);
  TEST_ASSERT_EQUAL(45, winterLocal.minute);
  TEST_ASSERT_EQUAL(60, winterLocal.offsetMinutes);  // CET = UTC+1

  DateTime summer = date.fromUtc(2024, 6, 1, 19, 30, 0);  // 21:30 CEST
  LocalDateTime summerLocal = date.toLocal(summer);
  TEST_ASSERT_TRUE(summerLocal.ok);
  TEST_ASSERT_EQUAL(2024, summerLocal.year);
  TEST_ASSERT_EQUAL(6, summerLocal.month);
  TEST_ASSERT_EQUAL(1, summerLocal.day);
  TEST_ASSERT_EQUAL(21, summerLocal.hour);
  TEST_ASSERT_EQUAL(30, summerLocal.minute);
  TEST_ASSERT_EQUAL(120, summerLocal.offsetMinutes);  // CEST = UTC+2
}

static void test_moon_phase_full_and_new_moon() {
  MoonPhaseResult full = date.moonPhase(date.fromUtc(2024, 3, 25, 0, 0, 0));  // full moon
  TEST_ASSERT_TRUE(full.ok);
  TEST_ASSERT_TRUE(full.illumination > 0.95);
  TEST_ASSERT_TRUE(full.angleDegrees >= 170 && full.angleDegrees <= 190);

  MoonPhaseResult solarEclipseNew = date.moonPhase(date.fromUtc(2024, 4, 8, 18, 0, 0));  // near new moon
  TEST_ASSERT_TRUE(solarEclipseNew.ok);
  TEST_ASSERT_TRUE(solarEclipseNew.illumination < 0.05);
  TEST_ASSERT_TRUE(solarEclipseNew.angleDegrees < 10 || solarEclipseNew.angleDegrees > 350);
}

void setUp() {}
void tearDown() {}

void setup() {
  setenv("TZ", "UTC", 1);
  tzset();
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_add_days_and_differences);
  RUN_TEST(test_add_months_clamps_day_in_leap_year);
  RUN_TEST(test_start_and_end_of_day_utc);
  RUN_TEST(test_parse_and_format_iso_utc);
  RUN_TEST(test_from_utc_clamps_day);
  RUN_TEST(test_start_of_year_helpers);
  RUN_TEST(test_next_daily_and_weekday_local);
  RUN_TEST(test_sunrise_config_matches_manual);
  RUN_TEST(test_is_day_helpers);
  RUN_TEST(test_is_dst_active_with_timezone_string);
  RUN_TEST(test_is_dst_active_with_configured_timezone);
  RUN_TEST(test_is_dst_active_with_system_timezone);
  RUN_TEST(test_to_local_breakdown);
  RUN_TEST(test_moon_phase_full_and_new_moon);
  UNITY_END();
}

void loop() {}
