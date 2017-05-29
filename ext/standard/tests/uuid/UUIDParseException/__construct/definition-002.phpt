--TEST--
UUIDParseException::__construct 1. parameter definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUIDParseException;

$p = (new ReflectionMethod(UUIDParseException::class, '__construct'))->getParameters()[0];

var_dump(
	$p->getName(),
	$p->allowsNull(),
	(string) $p->getType(),
	$p->isOptional(),
	$p->isPassedByReference()
);

?>
--EXPECT--
string(6) "reason"
bool(false)
string(6) "string"
bool(false)
bool(false)
