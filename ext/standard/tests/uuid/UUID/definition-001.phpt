--TEST--
UUID class definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

$c = new ReflectionClass(UUID::class);

var_dump(
	$c->getInterfaces() === [],
	$c->getParentClass(),
	$c->isAbstract(),
	$c->isCloneable(),
	$c->isFinal(),
	$c->isInstantiable(),
	$c->isInterface(),
	$c->isInternal(),
	$c->isIterateable(),
	$c->isTrait(),
	count($c->getConstants()),
	count($c->getProperties())
);

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
int(9)
int(1)
