--TEST--
PFA supports static methods
--FILE--
<?php
class Foo {
    public static function method($a, $b) {
        printf("%s\n", __METHOD__);
    }
}

$foo = Foo::method(new stdClass, ...);

$bar = $foo(new stdClass, ...);

$bar();
?>
--EXPECTF--
Foo::method
