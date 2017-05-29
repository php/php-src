--TEST--
UUIDParseException::__construct 4. parameter definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUIDParseException;

$p = (new ReflectionMethod(UUIDParseException::class, '__construct'))->getParameters()[3];

var_dump(
	$p->getName(),
	$p->allowsNull(),
	(string) $p->getType(),
	$p->isOptional(),
	//$p->getDefaultValue(),
	$p->isPassedByReference()
);

?>
--EXPECT--
string(8) "previous"
bool(true)
string(9) "Throwable"
bool(true)
bool(false)
