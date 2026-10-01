--TEST--
GH-23576 (Next index for array returned from array_keys() is wrong)
--FILE--
<?php
$a = [123 => 123];
unset($a[123]);

$b = array_keys($a);
$b[] = 42;
var_dump($b);

$c = array_keys($a, 123);
$c[] = 42;
var_dump($c);

$d = array_keys($a, 123, true);
$d[] = 42;
var_dump($d);

$e = [];

$f = array_keys($e);
$f[] = 42;
var_dump($f);

$g = array_keys($e, 123);
$g[] = 42;
var_dump($g);

$h = array_keys($e, 123, true);
$h[] = 42;
var_dump($h);
?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
array(1) {
  [0]=>
  int(42)
}
array(1) {
  [0]=>
  int(42)
}
array(1) {
  [0]=>
  int(42)
}
array(1) {
  [0]=>
  int(42)
}
array(1) {
  [0]=>
  int(42)
}
