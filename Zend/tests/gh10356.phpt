--TEST--
GH-10356: Incorrect line number of constant in constant expression
--FILE--
<?php

final class Foo {
    public static $foo = [
        'foo' => DOES_NOT_EXIST,
    ];
}

new Foo();

?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "DOES_NOT_EXIST" in %s:5
Stack trace:
#0 %s(%d): [constant expression]()
#1 {main}
  thrown in %s on line 5
