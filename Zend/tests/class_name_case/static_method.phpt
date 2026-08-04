--TEST--
Class name with incorrect case fails in static method call
--FILE--
<?php
class Foo {
    static function bar(): string { return 'ok'; }
}

echo Foo::bar() . "\n"; // correct case

try {
    echo FOO::bar() . "\n"; // wrong case
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    echo foo::bar() . "\n"; // wrong case
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
ok
Class "FOO" not found
Class "foo" not found
