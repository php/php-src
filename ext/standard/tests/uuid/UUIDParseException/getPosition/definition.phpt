--TEST--
UUIDParseException::getPosition method signature
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$m = new ReflectionMethod(UUIDParseException::class, 'getPosition');

var_dump(
	$m->getNumberOfParameters(),
	$m->getNumberOfRequiredParameters(),
	(string) $m->getReturnType(),
	$m->isAbstract(),
	$m->isPublic(),
	$m->isStatic()
);

?>
--EXPECT--
int(0)
int(0)
string(3) "int"
bool(false)
bool(true)
bool(false)
