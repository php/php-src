--TEST--
ReflectionClass::getFileName(), ReflectionClass::getStartLine(), ReflectionClass::getEndLine() - bad params
--FILE--
<?php
Class C { }

$rc = new ReflectionClass("C");
$methods = array("getFileName", "getStartLine", "getEndLine");

foreach ($methods as $method) {
	var_dump($rc->$method());
	var_dump($rc->$method(null));
	var_dump($rc->$method('X', 0));
}
?>
--EXPECTF--
string(%d) "%s"

Warning: ReflectionClass::getFileName() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getFileName() expects exactly 0 parameters, 2 given in %s on line %d
NULL
int(2)

Warning: ReflectionClass::getStartLine() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getStartLine() expects exactly 0 parameters, 2 given in %s on line %d
NULL
int(2)

Warning: ReflectionClass::getEndLine() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getEndLine() expects exactly 0 parameters, 2 given in %s on line %d
NULL
