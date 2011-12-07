--TEST--
Bug #52113 (Seg fault while creating (by unserialization) DatePeriod)
--INI--
date.timezone=UTC
--FILE--
<?php
$start = new DateTime('2003-01-02 08:00:00');
$end = new DateTime('2003-01-02 12:00:00');
$diff = $start->diff($end);
$p = new DatePeriod($start, $diff, 2);
$diff_s = serialize($diff);
var_dump($diff, $diff_s);
var_export($diff);

$diff_un = unserialize($diff_s);
$p = new DatePeriod($start, $diff_un, 2);
var_dump($diff_un, $p);

$unser = DateInterval::__set_state(array(
   'y' => 7,
   'm' => 6,
   'd' => 5,
   'h' => 4,
   'i' => 3,
   's' => 2,
   'invert' => 1,
   'days' => 2400,
));

$p = new DatePeriod($start, $diff_un, 2);
var_dump($unser, $p);

?>
--EXPECT--
object(DateInterval)#3 (8) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(0)
  ["h"]=>
  int(4)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(0)
}
string(128) "O:12:"DateInterval":8:{s:1:"y";i:0;s:1:"m";i:0;s:1:"d";i:0;s:1:"h";i:4;s:1:"i";i:0;s:1:"s";i:0;s:6:"invert";i:0;s:4:"days";i:0;}"
DateInterval::__set_state(array(
   'y' => 0,
   'm' => 0,
   'd' => 0,
   'h' => 4,
   'i' => 0,
   's' => 0,
   'invert' => 0,
   'days' => 0,
))object(DateInterval)#5 (8) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(0)
  ["h"]=>
  int(4)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(0)
}
object(DatePeriod)#6 (0) {
}
object(DateInterval)#4 (8) {
  ["y"]=>
  int(7)
  ["m"]=>
  int(6)
  ["d"]=>
  int(5)
  ["h"]=>
  int(4)
  ["i"]=>
  int(3)
  ["s"]=>
  int(2)
  ["invert"]=>
  int(1)
  ["days"]=>
  int(2400)
}
object(DatePeriod)#7 (0) {
}
