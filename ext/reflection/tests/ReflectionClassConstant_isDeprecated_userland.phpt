--TEST--
ReflectionClassConstant::isDeprecated() with userland constants.
--FILE--
<?php

class Clazz {
	#[\Deprecated]
	public const TEST = 'test';
}


$r = new ReflectionClassConstant('Clazz', 'TEST');
var_dump($r->isDeprecated());

?>
--EXPECTF--
bool(true)
