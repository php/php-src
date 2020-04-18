--TEST--
ReflectionClass::getInterfaces()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class A0 {}
class B0 extends A0 {}
abstract class A1 {}
class B1 extends A1 {}

interface I0 {}
interface I1 {}
interface I2 {}
interface I3 {}
interface I4 extends I3 {}
interface I5 extends I4 {}
interface I6 extends I5, I1, I2 {}
interface I7 extends I6 {}

class C0 implements I0 {}
class C1 implements I1, I3 {}
class C2 extends C1 {}
class C3 extends C2 implements I1 {}
class C4 extends C3 implements I2 {}
class C5 extends C4 implements I7 {}
class C6 implements I1, I2, I3, I4, I5, I6, I7 {}


$classes = array( 	'A0', 'A1', 'B0', 'B1',
                    'I0', 'I1', 'I2', 'I3', 'I4', 'I5', 'I6', 'I7',
                    'C0', 'C1', 'C2', 'C3', 'C4', 'C5', 'C6'	);

foreach ($classes as $class) {
    echo "---( Interfaces implemented by $class )---\n ";
    $rc = new ReflectionClass($class);
    $interfaces = $rc->getInterfaces();
    // Sort interfaces so that tests do not fail because of wrong order.
    ksort($interfaces);
    print_r($interfaces);
}

?>
--EXPECT--
---( Interfaces implemented by A0 )---
 Array
(
)
---( Interfaces implemented by A1 )---
 Array
(
)
---( Interfaces implemented by B0 )---
 Array
(
)
---( Interfaces implemented by B1 )---
 Array
(
)
---( Interfaces implemented by I0 )---
 Array
(
)
---( Interfaces implemented by I1 )---
 Array
(
)
---( Interfaces implemented by I2 )---
 Array
(
)
---( Interfaces implemented by I3 )---
 Array
(
)
---( Interfaces implemented by I4 )---
 Array
(
    [I3] => ReflectionClass Object
        (
            [name] => I3
        )

)
---( Interfaces implemented by I5 )---
 Array
(
    [I3] => ReflectionClass Object
        (
            [name] => I3
        )

    [I4] => ReflectionClass Object
        (
            [name] => I4
        )

)
---( Interfaces implemented by I6 )---
 Array
(
    [I1] => ReflectionClass Object
        (
            [name] => I1
        )

    [I2] => ReflectionClass Object
        (
            [name] => I2
        )

    [I3] => ReflectionClass Object
        (
            [name] => I3
        )

    [I4] => ReflectionClass Object
        (
            [name] => I4
        )

    [I5] => ReflectionClass Object
        (
            [name] => I5
        )

)
---( Interfaces implemented by I7 )---
 Array
(
    [I1] => ReflectionClass Object
        (
            [name] => I1
        )

    [I2] => ReflectionClass Object
        (
            [name] => I2
        )

    [I3] => ReflectionClass Object
        (
            [name] => I3
        )

    [I4] => ReflectionClass Object
        (
            [name] => I4
        )

    [I5] => ReflectionClass Object
        (
            [name] => I5
        )

    [I6] => ReflectionClass Object
        (
            [name] => I6
        )

)
---( Interfaces implemented by C0 )---
 Array
(
    [I0] => ReflectionClass Object
        (
            [name] => I0
        )

)
---( Interfaces implemented by C1 )---
 Array
(
    [I1] => ReflectionClass Object
        (
            [name] => I1
        )

    [I3] => ReflectionClass Object
        (
            [name] => I3
        )

)
---( Interfaces implemented by C2 )---
 Array
(
    [I1] => ReflectionClass Object
        (
            [name] => I1
        )

    [I3] => ReflectionClass Object
        (
            [name] => I3
        )

)
---( Interfaces implemented by C3 )---
 Array
(
    [I1] => ReflectionClass Object
        (
            [name] => I1
        )

    [I3] => ReflectionClass Object
        (
            [name] => I3
        )

)
---( Interfaces implemented by C4 )---
 Array
(
    [I1] => ReflectionClass Object
        (
            [name] => I1
        )

    [I2] => ReflectionClass Object
        (
            [name] => I2
        )

    [I3] => ReflectionClass Object
        (
            [name] => I3
        )

)
---( Interfaces implemented by C5 )---
 Array
(
    [I1] => ReflectionClass Object
        (
            [name] => I1
        )

    [I2] => ReflectionClass Object
        (
            [name] => I2
        )

    [I3] => ReflectionClass Object
        (
            [name] => I3
        )

    [I4] => ReflectionClass Object
        (
            [name] => I4
        )

    [I5] => ReflectionClass Object
        (
            [name] => I5
        )

    [I6] => ReflectionClass Object
        (
            [name] => I6
        )

    [I7] => ReflectionClass Object
        (
            [name] => I7
        )

)
---( Interfaces implemented by C6 )---
 Array
(
    [I1] => ReflectionClass Object
        (
            [name] => I1
        )

    [I2] => ReflectionClass Object
        (
            [name] => I2
        )

    [I3] => ReflectionClass Object
        (
            [name] => I3
        )

    [I4] => ReflectionClass Object
        (
            [name] => I4
        )

    [I5] => ReflectionClass Object
        (
            [name] => I5
        )

    [I6] => ReflectionClass Object
        (
            [name] => I6
        )

    [I7] => ReflectionClass Object
        (
            [name] => I7
        )

)
