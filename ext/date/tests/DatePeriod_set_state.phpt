--TEST--
Test __set_state magic method for recreating a DatePeriod object
--CREDITS--
Mark Baker mark@lange.demon.co.uk at the PHPNW2017 Conference for PHP Testfest 2017
--FILE--
<?php

$datePeriodObject = new DatePeriod(
    new DateTime('2017-10-06 23:30', new DateTimeZone('UTC')),
    new DateInterval('PT1H30M'),
    24
);

$datePeriodState = var_export($datePeriodObject, true);

eval("\$datePeriodObjectNew = {$datePeriodState};");

var_dump($datePeriodObjectNew);

?>
--EXPECTF--
object(DatePeriod)#%d (%d) {
  ["start"]=>
  object(DateTime)#%d (%d) {
    ["date"]=>
    string(26) "2017-10-06 23:30:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(3) "UTC"
  }
  ["current"]=>
  NULL
  ["end"]=>
  NULL
  ["interval"]=>
  object(DateTimeInterval)#%d (%d) {
    ["from_string"]=>
    bool(false)
    ["y"]=>
    int(0)
    ["m"]=>
    int(0)
    ["d"]=>
    int(0)
    ["h"]=>
    int(1)
    ["i"]=>
    int(30)
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
  int(25)
  ["include_start_date"]=>
  bool(true)
  ["include_end_date"]=>
  bool(false)
}
