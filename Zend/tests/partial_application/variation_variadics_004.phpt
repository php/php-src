--TEST--
PFA variation: variadics and optional args
--FILE--
<?php
function foo(int $day = 1, int $month = 1, int $year = 2005) {
    return ["day" => $day, "month" => $month, "year" => $year];
}

$foo = foo(year: 2006, ...);

var_dump($foo(2));

$foo = foo(month: 12, ...);

$bar = $foo(year: 2016, ...);

var_dump($foo(2));

var_dump($bar(2));

var_dump($foo());

var_dump($bar());
?>
--EXPECTF--
array(3) {
  ["day"]=>
  int(2)
  ["month"]=>
  int(1)
  ["year"]=>
  int(2006)
}
array(3) {
  ["day"]=>
  int(2)
  ["month"]=>
  int(12)
  ["year"]=>
  int(2005)
}
array(3) {
  ["day"]=>
  int(2)
  ["month"]=>
  int(12)
  ["year"]=>
  int(2016)
}
array(3) {
  ["day"]=>
  int(1)
  ["month"]=>
  int(12)
  ["year"]=>
  int(2005)
}
array(3) {
  ["day"]=>
  int(1)
  ["month"]=>
  int(12)
  ["year"]=>
  int(2016)
}
