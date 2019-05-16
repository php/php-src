--TEST--
Bug #22725 (A derived class can call a parent's protected method that calls a private method)
--FILE--
<?php
class Foo {
    private function aPrivateMethod() {
        echo "Foo::aPrivateMethod() called.\n";
    }

    protected function aProtectedMethod() {
        echo "Foo::aProtectedMethod() called.\n";
        $this->aPrivateMethod();
    }
}

class Bar extends Foo {
    public function aPublicMethod() {
        echo "Bar::aPublicMethod() called.\n";
        $this->aProtectedMethod();
    }
}

$o = new Bar;
$o->aPublicMethod();
?>
--EXPECT--
Bar::aPublicMethod() called.
Foo::aProtectedMethod() called.
Foo::aPrivateMethod() called.
