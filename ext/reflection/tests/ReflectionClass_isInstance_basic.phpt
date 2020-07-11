--TEST--
ReflectionClass::isInstance()
--FILE--
<?php
class A {}
class B extends A {}

interface I {}
class C implements I {}

class X {}

$classes = array("A", "B", "C", "I", "X");

$instances = array(	"myA" => new A,
                    "myB" => new B,
                    "myC" => new C,
                    "myX" => new X );

foreach ($classes as $class) {
    $rc = new ReflectionClass($class);

    foreach ($instances as $name => $instance) {
        echo "is $name a $class? ";
        var_dump($rc->isInstance($instance));
    }

}

?>
--EXPECT--
is myA a A? bool(true)
is myB a A? bool(true)
is myC a A? bool(false)
is myX a A? bool(false)
is myA a B? bool(false)
is myB a B? bool(true)
is myC a B? bool(false)
is myX a B? bool(false)
is myA a C? bool(false)
is myB a C? bool(false)
is myC a C? bool(true)
is myX a C? bool(false)
is myA a I? bool(false)
is myB a I? bool(false)
is myC a I? bool(true)
is myX a I? bool(false)
is myA a X? bool(false)
is myB a X? bool(false)
is myC a X? bool(false)
is myX a X? bool(true)
