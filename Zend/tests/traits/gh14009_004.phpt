--TEST--
GH-14009: Traits inherit prototype
--FILE--
<?php

class A {
    protected function test() {
        throw new Exception('Unreachable');
    }
}

class B extends A {
    // Prototype is A::test().
    protected function test() {
        echo __METHOD__, "\n";
    }
}

trait T {
    protected abstract function test();
}

class C extends B {
    use T;
}

class D extends A {
    public static function callTest($sibling) {
        $sibling->test();
    }
}

D::callTest(new C());

?>
--EXPECT--
B::test
