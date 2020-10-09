--TEST--
Bug #79364 (When copy empty array, next key is unspecified)
--FILE--
<?php
$a = [1, 2];
unset($a[1], $a[0]);
$b = $a;

$a[] = 3;
$b[] = 4;

var_dump($a, $b);
?>
--EXPECT--
array(1) {
  [2]=>
  int(3)
}
array(1) {
  [2]=>
  int(4)
}
