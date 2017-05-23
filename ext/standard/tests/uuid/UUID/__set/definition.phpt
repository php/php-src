--TEST--
UUID::__set method signature
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$m = new ReflectionMethod(UUID::class, '__set');

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
string(4) "void"
bool(true)
bool(false)
