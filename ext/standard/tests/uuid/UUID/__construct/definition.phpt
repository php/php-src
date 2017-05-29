--TEST--
UUID::__construct method signature
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

$m = new ReflectionMethod(UUID::class, '__construct');

var_dump(
	$m->getNumberOfParameters(),
	$m->getNumberOfRequiredParameters(),
	$m->hasReturnType(),
	$m->isPrivate(),
	$m->isStatic()
);

?>
--EXPECT--
int(0)
int(0)
bool(false)
bool(true)
bool(false)
