--TEST--
UUID::fromBinary 1. parameter definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

$p = (new ReflectionMethod(UUID::class, 'fromBinary'))->getParameters()[0];

var_dump(
	$p->getName(),
	$p->allowsNull(),
	(string) $p->getType(),
	$p->isOptional(),
	$p->isPassedByReference(),
	$p->isVariadic()
);

?>
--EXPECT--
string(5) "input"
bool(false)
string(6) "string"
bool(false)
bool(false)
bool(false)
