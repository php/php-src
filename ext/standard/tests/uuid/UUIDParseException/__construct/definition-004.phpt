--TEST--
UUIDParseException::__construct 3. parameter definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUIDParseException;

$p = (new ReflectionMethod(UUIDParseException::class, '__construct'))->getParameters()[2];

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
string(8) "position"
bool(false)
string(3) "int"
bool(true)
bool(false)
