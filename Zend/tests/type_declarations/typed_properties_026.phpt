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
Fatal error: Typed property Baz::$baz must be integer, null used in %s on line 6






