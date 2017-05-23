--TEST--
UUIDParsingException::__construct method signature
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$m = new ReflectionMethod(UUIDParsingException::class, '__construct');

var_dump(
    $m->getNumberOfParameters(),
    $m->getNumberOfRequiredParameters(),
    $m->hasReturnType(),
    $m->isPublic()
);

?>
--EXPECT--
int(4)
int(2)
bool(false)
bool(true)
