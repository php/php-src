--TEST--
ReflectionMethod::isDeprecated(): Implementing a deprecated abstract method.
--FILE--
<?php

abstract class P {
	#[\Deprecated]
	abstract function test();
}

class Clazz extends P {
	function test() {
	}
}


$c = new Clazz();
$c->test();

$r = new ReflectionMethod('P', 'test');
var_dump($r->isDeprecated());

$r = new ReflectionMethod('Clazz', 'test');
var_dump($r->isDeprecated());

?>
--EXPECTF--
bool(true)
bool(false)
