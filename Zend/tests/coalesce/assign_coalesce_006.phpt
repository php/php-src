--TEST--
Null coalesce assign with memoized constant operand that is later interned (OSS-Fuzz #17903)
--FILE--
<?php
$foo[__DIR__] ??= 42;
var_dump($foo);
?>
--EXPECTF--
array(1) {
  ["%s"]=>
  int(42)
}
