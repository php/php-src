--TEST--
UUIDParseException::__construct method signature
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUIDParseException;

$m = new ReflectionMethod(UUIDParseException::class, '__construct');

var_dump(
	$m->getNumberOfParameters(),
	$m->getNumberOfRequiredParameters(),
	$m->hasReturnType(),
	$m->isPublic()
);

?>
--EXPECT--
int(4)
int(2)
bool(false)
bool(true)
