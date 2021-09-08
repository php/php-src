--TEST--
Properties of DateTime class
--FILE--
<?php

var_dump(get_class_vars(DateTime::class));

$datetime = new DateTime('2000-01-02 03:04:05.100000Z');
var_dump(get_object_vars($datetime));

assert(isset(
    $datetime->year,
    $datetime->month,
    $datetime->day,
    $datetime->hour,
    $datetime->minute,
    $datetime->second,
    $datetime->microsecond,
));


$neg_datetime = new DateTime('-2000-01-02 03:04:05.100000Z');
var_dump($neg_datetime->year);

?>
--EXPECT--
array(7) {
  ["year"]=>
  NULL
  ["month"]=>
  NULL
  ["day"]=>
  NULL
  ["hour"]=>
  NULL
  ["minute"]=>
  NULL
  ["second"]=>
  NULL
  ["microsecond"]=>
  NULL
}
array(7) {
  ["year"]=>
  int(2000)
  ["month"]=>
  int(1)
  ["day"]=>
  int(2)
  ["hour"]=>
  int(3)
  ["minute"]=>
  int(4)
  ["second"]=>
  int(5)
  ["microsecond"]=>
  int(100000)
}
int(-2000)