--TEST--
Testing function call through of array item
--FILE--
<?php

$arr = array('strtoupper', 'strtolower');

$k = 0;

var_dump($arr[0]('foo') == 'FOO');
var_dump($arr[$k]('foo') == 'FOO');
var_dump($arr[++$k]('FOO') == 'foo');
var_dump($arr[++$k]('FOO') == 'foo');

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)

Warning: Undefined array key 2 in %s on line %d

Fatal error: Uncaught Error: Value of type null is not callable in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
