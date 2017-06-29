--TEST--
UUIDParseException::__construct 2. parameter definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUIDParseException;

$p = (new ReflectionMethod(UUIDParseException::class, '__construct'))->getParameters()[1];

var_dump(
	$p->getName(),
	$p->allowsNull(),
	(string) $p->getType(),
	$p->isOptional(),
	$p->isPassedByReference()
);

?>
--EXPECT--
string(5) "input"
bool(false)
string(6) "string"
bool(false)
bool(false)
