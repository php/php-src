--TEST--
Test typed properties inherit traits with typed properties
--FILE--
<?php
trait Foo{
    private int $baz;
}

class Baz{
    use Foo;

    function get(){
        return $this->baz;
    }
}

var_dump((new Baz)->get());
--EXPECTF--
Fatal error: Uncaught Error: Typed property Baz::$baz must not be accessed before initialization in %s:10
Stack trace:
#0 %s(14): Baz->get()
#1 {main}
  thrown in %s on line 10
