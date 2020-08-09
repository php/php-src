--TEST--
Bug #38772 (inconsistent overriding of methods in different visibility contexts)
--FILE--
<?php
class A {

    public function __construct() {
        $this -> foo();
    }

    private function foo() {
        echo __METHOD__ . "\r\n";
    }
}

class B extends A {
    public function foo() {
        echo __METHOD__ . "\r\n";
    }
}

class C extends A {
    protected function foo() {
        echo __METHOD__ . "\r\n";
    }
}

class D extends A {
        private function foo() {
                echo __METHOD__ . "\r\n";
        }
}

$a = new A();
$b = new B();
$c = new C();
$d = new D();
?>
--EXPECT--
A::foo
A::foo
A::foo
A::foo
