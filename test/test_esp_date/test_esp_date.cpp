#include <Arduino.h>
#include <ESPDate.h>
#include <unity.h>
#include <time.h>

static ESPDate date;

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
  UNITY_END();
}

void loop() {}
