--TEST--
Bug #72920 (Accessing a private constant using constant() creates an exception AND warning)
--FILE--
<?php
class Foo {
    private const C1 = "a";
}

try {
    var_dump(constant('Foo::C1'));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Private constant Foo::C1 cannot be accessed from the global scope
