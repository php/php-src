--TEST--
Test ReflectionProperty::isReadable() unrelated object
--FILE--
<?php

class A {
}

class B extends A {
    public $prop;
}

class C extends B {}

class D {}

function test($obj) {
    $r = new ReflectionProperty('B', 'prop');
    try {
        var_dump($r->isReadable(null, $obj));
    } catch (Exception $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

test(new A);
test(new B);
test(new C);
test(new D);

?>
--EXPECT--
ReflectionException: Given object is not an instance of the class this property was declared in
bool(true)
bool(true)
ReflectionException: Given object is not an instance of the class this property was declared in
