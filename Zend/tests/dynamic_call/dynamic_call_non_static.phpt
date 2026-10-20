--TEST--
Dynamic static call of non-static method
--FILE--
<?php
class Foo {
    function test1() {
        $method = ['Foo', 'bar'];
        $method();
    }
    function test2() {
        $method = 'Foo::bar';
        $method();
    }
    function __call($name, $args) {}
}
$x = new Foo;
try {
    $x->test1();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $x->test2();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Non-static method Foo::bar() cannot be called statically
Error: Non-static method Foo::bar() cannot be called statically
