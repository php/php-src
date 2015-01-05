--TEST--
ReflectionParameter Check for parameter being optional
--FILE--
<?php

class Test {
	function func($x, $y = NULL){
	}
}


$f = new ReflectionMethod('Test', 'func');
var_dump($f->getNumberOfParameters());
var_dump($f->getNumberOfRequiredParameters());

$p = new ReflectionParameter(array('Test', 'func'), 'x');
var_dump($p->isOptional());

$p = new ReflectionParameter(array('Test', 'func'), 'y');
var_dump($p->isOptional());

try {
	$p = new ReflectionParameter(array('Test', 'func'), 'z');
	var_dump($p->isOptional());
}
catch (Exception $e) {
	var_dump($e->getMessage());
}

?>
===DONE===
--EXPECT--
int(2)
int(1)
bool(false)
bool(true)
string(54) "The parameter specified by its name could not be found"
===DONE===
