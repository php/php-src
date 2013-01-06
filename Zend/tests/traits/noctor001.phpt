--TEST--
Don't mark trait methods as constructor
--FILE--
<?php
trait Foo {
    public function Foo() {
    }
}

class Bar {
    use Foo;
    public function Bar() {
    }
}

$rfoofoo = new ReflectionMethod('Foo::Foo');
var_dump($rfoofoo->isConstructor());

$rbarfoo = new ReflectionMethod('Bar::Foo');
var_dump($rbarfoo->isConstructor());

$rbarbar = new ReflectionMethod('Bar::Bar');
var_dump($rbarbar->isConstructor());
?>
--EXPECT--
bool(false)
bool(false)
bool(true)
