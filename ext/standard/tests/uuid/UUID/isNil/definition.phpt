--TEST--
UUID::isNil method signature
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$m = new ReflectionMethod(UUID::class, 'isNil');

var_dump(
	$m->getNumberOfParameters(),
	$m->getNumberOfRequiredParameters(),
	(string) $m->getReturnType(),
	$m->isPublic(),
	$m->isStatic()
);

?>
--EXPECT--
int(0)
int(0)
string(4) "bool"
bool(true)
bool(false)
