--TEST--
UUID::v5 1. parameter definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

$p = (new ReflectionMethod(UUID::class, 'v5'))->getParameters()[0];

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
string(9) "namespace"
bool(false)
string(4) "self"
bool(false)
bool(false)
bool(false)
