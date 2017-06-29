--TEST--
UUIDParseException class definition.
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUIDParseException;

$c = new ReflectionClass(UUIDParseException::class);

var_dump(
	$c->isAbstract(),
	$c->isFinal(),
	$c->isInstantiable(),
	$c->isInternal(),
	$c->isSubclassOf(Exception::class)
);

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
