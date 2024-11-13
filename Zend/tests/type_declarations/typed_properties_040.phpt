--TEST--
Test __get on unset typed property must fail properly
--FILE--
<?php
declare(strict_types=1);

class Foo {
    public int $bar;

    public function __get($name) {
        var_dump($name);
    }
}

$foo = new Foo();

unset($foo->bar);
var_dump($foo->bar);
?>
--EXPECTF--
string(3) "bar"

Fatal error: Uncaught TypeError: Value of type null returned from Foo::__get() must be compatible with unset property Foo::$bar of type int in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
