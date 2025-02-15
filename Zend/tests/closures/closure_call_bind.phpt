--TEST--
Calling bindTo() on __call() closure
--FILE--
<?php

class Foo {
    function __call($name, $args) {
        echo "__call($name)\n";
    }
}

$foo = new Foo;
$name = "foo";
Closure::fromCallable([$foo, $name . "bar"])->bindTo(new Foo)();
$foo->{$name . "bar"}(...)->bindTo(new Foo)();

?>
--EXPECT--
__call(foobar)
__call(foobar)
