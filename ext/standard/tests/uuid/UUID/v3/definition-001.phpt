--TEST--
UUID::v3 method signature
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

$m = new ReflectionMethod(UUID::class, 'v3');

var_dump(
	$m->getNumberOfParameters(),
	$m->getNumberOfRequiredParameters(),
	(string) $m->getReturnType(),
	$m->isPublic(),
	$m->isStatic()
);

?>
--EXPECT--
int(2)
int(2)
string(4) "self"
bool(true)
bool(true)
