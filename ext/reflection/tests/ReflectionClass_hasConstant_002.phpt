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
var_dump($rc->hasConstant(null));
var_dump($rc->hasConstant(1));
var_dump($rc->hasConstant(1.5));
var_dump($rc->hasConstant(true));
?>
--EXPECT--
Check invalid params:
bool(false)
bool(false)
bool(false)
bool(false)
