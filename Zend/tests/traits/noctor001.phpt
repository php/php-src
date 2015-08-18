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
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; Bar has a deprecated constructor in %s on line %d
bool(false)
bool(false)
bool(true)
