--TEST--
UUIDParseException::getInput method signature
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUIDParseException;

$m = new ReflectionMethod(UUIDParseException::class, 'getInput');

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
string(6) "string"
bool(false)
bool(true)
bool(false)
