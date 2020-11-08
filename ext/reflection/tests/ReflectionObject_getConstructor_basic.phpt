--TEST--
ReflectionObject::getConstructor() - basic function test
--FILE--
<?php
class NewCtor {
    function __construct() {}
}

class ExtendsNewCtor extends NewCtor {
}

class X {
    function Y() {}
}

class Y extends X {
}

class B {
    function B() {}
}

class C extends B {
    function C() {}
}

class D1 extends C {
    function __construct() {}
}

class D2 extends C {
}

$classes = array('NewCtor', 'ExtendsNewCtor',
                 'B', 'C', 'D1', 'D2', 'X', 'Y');

foreach ($classes as $class) {
    $rc = new ReflectionObject(new $class);
    $rm = $rc->getConstructor();
    if ($rm != null) {
        echo "Constructor of $class: " . $rm->getName() . "\n";
    }  else {
        echo "No constructor for $class\n";
    }

}

?>
--EXPECT--
Constructor of NewCtor: __construct
Constructor of ExtendsNewCtor: __construct
No constructor for B
No constructor for C
Constructor of D1: __construct
No constructor for D2
No constructor for X
No constructor for Y
