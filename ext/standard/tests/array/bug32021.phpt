--TEST--
Bug #32021 (Crash caused by range('', 'z'))
--FILE--
<?php
$foo = range('', 'z');
var_dump($foo);
?>
ALIVE
--EXPECT--
array(1) {
  [0]=>
  int(0)
}
ALIVE
