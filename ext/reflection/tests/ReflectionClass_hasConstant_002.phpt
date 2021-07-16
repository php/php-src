--TEST--
ReflectionClass::hasConstant() - error cases
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
    const myConst = 1;
}

$rc = new ReflectionClass("C");
echo "Check invalid params:\n";
var_dump($rc->hasConstant(1));
var_dump($rc->hasConstant(1.5));
var_dump($rc->hasConstant(true));
?>
--EXPECTF--
Check invalid params:
bool(false)
bool(false)

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
bool(false)
