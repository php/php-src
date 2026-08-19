--TEST--
Bug #72920 (Accessing a private constant using constant() creates an exception AND warning)
--FILE--
<?php
class Foo {
    private const C1 = "a";
}

try {
    var_dump(constant('Foo::C1'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Cannot access private constant Foo::C1
