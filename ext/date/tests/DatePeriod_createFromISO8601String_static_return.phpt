--TEST--
Test the static return type of DatePeriod::createFromISO8601String()
--FILE--
<?php

class MyDatePeriod extends DatePeriod {}

var_dump(MyDatePeriod::createFromISO8601String("R4/2012-07-01T00:00:00Z/P7D"));

try {
    MyDatePeriod::createFromISO8601String("R4/2012-07-01T00:/P7D");
} catch (DateMalformedPeriodStringException $e) {
    echo $e->getMessage() . "\n";
}

try {
    MyDatePeriod::createFromISO8601String("R4/2012-07-01T00:00:00Z");
} catch (DateMalformedPeriodStringException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
object(MyDatePeriod)#1 (7) {
  ["start"]=>
  object(DateTimeImmutable)#2 (3) {
    ["date"]=>
    string(26) "2012-07-01 00:00:00.000000"
    ["timezone_type"]=>
    int(1)
    ["timezone"]=>
    string(6) "+00:00"
  }
  ["current"]=>
  NULL
  ["end"]=>
  NULL
  ["interval"]=>
  object(DateInterval)#3 (10) {
    ["y"]=>
    int(0)
    ["m"]=>
    int(0)
    ["d"]=>
    int(7)
    ["h"]=>
    int(0)
    ["i"]=>
    int(0)
    ["s"]=>
    int(0)
    ["f"]=>
    float(0)
    ["invert"]=>
    int(0)
    ["days"]=>
    bool(false)
    ["from_string"]=>
    bool(false)
  }
  ["recurrences"]=>
  int(5)
  ["include_start_date"]=>
  bool(true)
  ["include_end_date"]=>
  bool(false)
}
Unknown or bad format (R4/2012-07-01T00:/P7D)
DatePeriod::createFromISO8601String(): ISO interval must contain an interval, "R4/2012-07-01T00:00:00Z" given
