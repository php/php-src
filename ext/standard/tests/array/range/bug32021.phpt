--TEST--
Bug #32021 (Crash caused by range('', 'z'))
--FILE--
<?php
$foo = range('', 'z');
var_dump($foo);
?>
ALIVE
--EXPECTF--
Warning: range(): Argument #1 ($start) must not be empty, casted to 0 in %s on line %d

Warning: range(): Argument #1 ($start) must be a single byte string if argument #2 ($end) is a single byte string, argument #2 ($end) converted to 0 in %s on line %d
array(1) {
  [0]=>
  int(0)
}
ALIVE
