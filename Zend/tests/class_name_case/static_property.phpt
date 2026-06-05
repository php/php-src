--TEST--
Class name with incorrect case fails in static property access
--FILE--
<?php
class Foo {
    static int $x = 42;
}

var_dump(Foo::$x);   // correct case

try {
    var_dump(FOO::$x);   // wrong case
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(foo::$x);   // wrong case
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
int(42)
Class "FOO" not found
Class "foo" not found
