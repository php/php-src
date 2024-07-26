--TEST--
SplFixedArray indirect modification by fetching reference
--FILE--
<?php
$a = new SplFixedArray(1);
try {
    $r = &$a[0];
    $r = 3;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($a);
unset($r);
var_dump($a);
$v = $a[0];
var_dump($v);
$v = 10;
var_dump($v);
var_dump($a);
?>
--EXPECT--
object(SplFixedArray)#1 (1) {
  [0]=>
  &int(3)
}
object(SplFixedArray)#1 (1) {
  [0]=>
  &int(3)
}
int(3)
int(10)
object(SplFixedArray)#1 (1) {
  [0]=>
  &int(3)
}
