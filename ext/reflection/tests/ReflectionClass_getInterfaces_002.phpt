--TEST--
ReflectionClass::getInterfaces() - interface ordering.
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
interface I1 {}
interface I2 {}
interface I3 {}
interface I4 extends I3 {}
interface I5 extends I4 {}
interface I6 extends I5, I1, I2 {}
interface I7 extends I6 {}

$rc = new ReflectionClass('I7');
$interfaces = $rc->getInterfaces();
print_r($interfaces);
?>
--EXPECTF--
Array
(
    [I6] => ReflectionClass Object
        (
            [name] => I6
        )

    [I2] => ReflectionClass Object
        (
            [name] => I2
        )

    [I1] => ReflectionClass Object
        (
            [name] => I1
        )

    [I4] => ReflectionClass Object
        (
            [name] => I4
        )

    [I3] => ReflectionClass Object
        (
            [name] => I3
        )

    [I5] => ReflectionClass Object
        (
            [name] => I5
        )

)