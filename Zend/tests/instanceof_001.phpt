--TEST--
Testing instanceof operator with several operators
--FILE--
<?php 

$a = new stdClass;
var_dump($a instanceof stdClass);

var_dump(new stdCLass instanceof stdClass);

$b = create_function('', 'return new stdClass;');
var_dump($b() instanceof stdClass);

$c = array(new stdClass);
var_dump($c[0] instanceof stdClass);

var_dump(@$inexistent instanceof stdClass);

var_dump("$a" instanceof stdClass);

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)

Catchable fatal error: Object of class stdClass could not be converted to string in %s on line %d
