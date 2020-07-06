--TEST--
Bug #69467 (Wrong checked for the interface by using Trait)
--FILE--
<?php
interface Baz {
    public function bad();
}

trait Bar{
    protected function bad(){}
}

class Foo implements Baz{
    use Bar;
}

$test = new Foo();
var_dump($test instanceof Baz);
?>
--EXPECTF--
Fatal error: Method Foo::bad() must have public visibility to be compatible with overridden method Baz::bad() in %s on line %d
