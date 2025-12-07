#include <Arduino.h>
#include <ESPDate.h>

ESPDate date;

void printFormatted(const char *label, const DateTime &dt) {
  char buf[32];
  if (date.formatUtc(dt, ESPDateFormat::Iso8601, buf, sizeof(buf))) {
    Serial.print(label);
    Serial.println(buf);
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("ESPDate basic example");
  Serial.println("Set the system clock (SNTP/manual) before relying on date.now().");

  DateTime now = date.now();
  DateTime tomorrow = date.addDays(now, 1);
  DateTime lastWeek = date.subDays(now, 7);

  printFormatted("Now (UTC): ", now);
  printFormatted("Tomorrow (UTC): ", tomorrow);
  printFormatted("Last week (UTC): ", lastWeek);

  int64_t deltaDays = date.differenceInDays(tomorrow, now);
  Serial.printf("Days between now and tomorrow: %lld\n", static_cast<long long>(deltaDays));

  DateTime start = date.startOfDayLocal(now);
  DateTime end = date.endOfDayLocal(now);

  char buf[32];
  if (date.formatLocal(start, ESPDateFormat::DateTime, buf, sizeof(buf))) {
    Serial.print("Local day starts at: ");
    Serial.println(buf);
  }
  if (date.formatLocal(end, ESPDateFormat::DateTime, buf, sizeof(buf))) {
    Serial.print("Local day ends at: ");
    Serial.println(buf);
  }

  ESPDate::ParseResult parsed = date.parseIso8601Utc("2025-12-31T23:59:30Z");
  if (parsed.ok) {
    Serial.print("Parsed ISO-8601 (UTC): ");
    date.formatUtc(parsed.value, ESPDateFormat::DateTime, buf, sizeof(buf));
    Serial.println(buf);
  }

  ESPDate::ParseResult parsedLocal = date.parseDateTimeLocal("2025-01-01 03:00:00");
  if (parsedLocal.ok) {
    Serial.print("Parsed local datetime: ");
    if (date.formatLocal(parsedLocal.value, ESPDateFormat::DateTime, buf, sizeof(buf))) {
      Serial.println(buf);
    }
  }
}

void loop() {
  // Intentionally empty.
}
