--TEST--
Testing anonymous function return as array key and accessing $GLOBALS
--FILE--
<?php

$test = create_function('$v', 'return $v;');

$arr = array(create_function('', 'return $GLOBALS["arr"];'), 2);

var_dump($arr[$test(1)]);
var_dump($arr[$test(0)]() == $arr);

?>
--EXPECTF--
Deprecated: Function create_function() is deprecated in %s on line %d

Deprecated: Function create_function() is deprecated in %s on line %d
int(2)
bool(true)
