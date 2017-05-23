--TEST--
UUID::v3 2. parameter definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$p = (new ReflectionMethod(UUID::class, 'v3'))->getParameters()[1];

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
string(4) "name"
bool(false)
string(6) "string"
bool(false)
bool(false)
bool(false)
