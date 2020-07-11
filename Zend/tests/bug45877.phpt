--TEST--
Bug #45877 (Array key '2147483647' left as string)
--INI--
precision=16
--FILE--
<?php
$keys = array(PHP_INT_MAX,
    (string) PHP_INT_MAX,
    (string) (-PHP_INT_MAX - 1),
    -PHP_INT_MAX - 1,
    (string) (PHP_INT_MAX + 1));

var_dump(array_fill_keys($keys, 1));
?>
--EXPECTF--
array(3) {
  [%d7]=>
  int(1)
  [-%d8]=>
  int(1)
  ["%s"]=>
  int(1)
}
