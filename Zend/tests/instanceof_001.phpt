--TEST--
Testing instanceof operator with several operators
--FILE--
<?php

$a = new StdClass;
var_dump($a instanceof StdClass);

var_dump(new StdClass instanceof StdClass);

$b = function() { return new StdClass; };
var_dump($b() instanceof StdClass);

$c = array(new StdClass);
var_dump($c[0] instanceof StdClass);

var_dump(@$inexistent instanceof StdClass);

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
