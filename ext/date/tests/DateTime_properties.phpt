--TEST--
Properties of DateTime class
--FILE--
<?php

var_dump(get_class_vars(DateTime::class));

$datetime = new DateTime('2000-01-02 03:04:05.100000Z');
var_dump(get_object_vars($datetime));

echo "\nvar_dump\n";
var_dump($datetime);

echo "\nprint_r\n";
print_r($datetime);

echo "\nvar_export\n";
var_export($datetime);

echo "\nArray Cast\n";
var_dump((array) $datetime);

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

var_dump
object(DateTime)#1 (10) {
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
  ["date"]=>
  string(26) "2000-01-02 03:04:05.100000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(1) "Z"
}

print_r
DateTime Object
(
    [year] => 2000
    [month] => 1
    [day] => 2
    [hour] => 3
    [minute] => 4
    [second] => 5
    [microsecond] => 100000
    [date] => 2000-01-02 03:04:05.100000
    [timezone_type] => 2
    [timezone] => Z
)

var_export
DateTime::__set_state(array(
   'date' => '2000-01-02 03:04:05.100000',
   'timezone_type' => 2,
   'timezone' => 'Z',
))
Array Cast
array(10) {
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
  ["date"]=>
  string(26) "2000-01-02 03:04:05.100000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(1) "Z"
}
int(-2000)