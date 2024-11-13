--TEST--
OSS Fuzz #60734: use-after-free visible in ASAN build pre increment.
--FILE--
<?php
class Foo{
}
$test = new Foo;
$y = ++$test;
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot increment Foo in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
