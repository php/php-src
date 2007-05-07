--TEST--
Bug #22725 (A derived class can call a parent's protected method that calls a private method)
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
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
