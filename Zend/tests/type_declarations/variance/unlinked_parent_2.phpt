--TEST--
Using an unlinked parent interface
--FILE--
<?php

spl_autoload_register(function($class) {
    class X implements B {}
});

try {
    interface B extends A {
    }
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Interface "B" not found
