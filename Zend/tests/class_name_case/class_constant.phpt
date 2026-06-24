--TEST--
Class name with incorrect case fails in class constant access
--FILE--
<?php
class Foo {
    const BAR = 42;
}

var_dump(Foo::BAR);   // correct case

try {
    var_dump(FOO::BAR);   // wrong case
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(foo::BAR);   // wrong case
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
int(42)
Class "FOO" not found
Class "foo" not found
