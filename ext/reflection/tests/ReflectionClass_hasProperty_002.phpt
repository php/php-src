--TEST--
ReflectionClass::hasProperty() - error cases
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
    public $a;
}

$rc = new ReflectionClass("C");
echo "Check invalid params:\n";
var_dump($rc->hasProperty(null));
var_dump($rc->hasProperty(1));
var_dump($rc->hasProperty(1.5));
var_dump($rc->hasProperty(true));
?>
--EXPECT--
Check invalid params:
bool(false)
bool(false)
bool(false)
bool(false)
