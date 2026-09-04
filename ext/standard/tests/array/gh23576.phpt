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
