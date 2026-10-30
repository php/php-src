--TEST--
Test typed properties unset __get magical magic
--FILE--
<?php
class Foo {
    public int $bar;

    public function __get($name) {
        return "violate";
    }
}

$foo = new Foo;

$foo->bar = "1"; # ok

unset($foo->bar); # ok

var_dump($foo->bar); # not okay, __get is nasty
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Value of type string returned from Foo::__get() must be compatible with unset property Foo::$bar of type int in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
