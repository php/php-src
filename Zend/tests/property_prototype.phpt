--TEST--
Prototype based protected visibility check for properties
--FILE--
<?php

class A {
    protected $foo = 'A';
}

class B1 extends A {
    public function test($obj) {
        var_dump($obj->foo);
    }
}

class B2 extends A {
    protected $foo = 'B2';
}

(new B1)->test(new B2);

?>
--EXPECT--
string(2) "B2"
