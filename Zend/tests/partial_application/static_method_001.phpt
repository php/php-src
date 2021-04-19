--TEST--
Partial application static method
--FILE--
<?php
class Foo {
    public static function method() {
        printf("%s\n", __METHOD__);
    }
}

$foo = Foo::method(...);

$bar = $foo(...);

$bar();
?>
--EXPECTF--
Foo::method
