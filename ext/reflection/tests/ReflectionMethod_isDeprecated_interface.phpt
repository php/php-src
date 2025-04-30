--TEST--
ReflectionMethod::isDeprecated(): Implementing a deprecated interface method.
--FILE--
<?php

interface I {
	#[\Deprecated]
	function test();
}

class Clazz implements I {
	function test() {
	}
}


$c = new Clazz();
$c->test();

$r = new ReflectionMethod('I', 'test');
var_dump($r->isDeprecated());

$r = new ReflectionMethod('Clazz', 'test');
var_dump($r->isDeprecated());

?>
--EXPECTF--
bool(true)
bool(false)
