--TEST--
Test that DatePeriod::__set_state() can be called directly
--FILE--
<?php

$period = DatePeriod::__set_state(
    [
        "start" => new DateTime("2022-07-14 12:00:00", new DateTimeZone("UTC")),
        "end" => new DateTime("2022-07-16 12:00:00", new DateTimeZone("UTC")),
        "current" => null,
        "interval" => new DateInterval('P2D'),
        "recurrences" => 2,
        "include_start_date" => false,
        "include_end_date" => true,
    ]
);

var_dump($period);

?>
--EXPECTF--
object(DatePeriod)#%d (%d) {
  ["start"]=>
  object(DateTime)#%d (%d) {
    ["date"]=>
    string(26) "2022-07-14 12:00:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(3) "UTC"
  }
  ["current"]=>
  NULL
  ["end"]=>
  object(DateTime)#%d (%d) {
    ["date"]=>
    string(26) "2022-07-16 12:00:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(3) "UTC"
  }
  ["interval"]=>
  object(DateTimeInterval)#%d (%d) {
    ["from_string"]=>
    bool(false)
    ["y"]=>
    int(0)
    ["m"]=>
    int(0)
    ["d"]=>
    int(2)
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
  }
  ["recurrences"]=>
  int(2)
  ["include_start_date"]=>
  bool(false)
  ["include_end_date"]=>
  bool(true)
}
