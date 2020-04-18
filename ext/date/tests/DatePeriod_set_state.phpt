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
object(DatePeriod)#%d (6) {
  ["start"]=>
  object(DateTime)#%d (3) {
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
  object(DateInterval)#%d (16) {
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
    ["weekday"]=>
    int(0)
    ["weekday_behavior"]=>
    int(0)
    ["first_last_day_of"]=>
    int(0)
    ["invert"]=>
    int(0)
    ["days"]=>
    bool(false)
    ["special_type"]=>
    int(0)
    ["special_amount"]=>
    int(0)
    ["have_weekday_relative"]=>
    int(0)
    ["have_special_relative"]=>
    int(0)
  }
  ["recurrences"]=>
  int(25)
  ["include_start_date"]=>
  bool(true)
}
