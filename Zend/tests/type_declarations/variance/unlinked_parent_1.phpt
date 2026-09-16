--TEST--
Using an unlinked parent class
--FILE--
<?php

spl_autoload_register(function($class) {
    class X extends B {}
});

try {
    class B extends A {
    }
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Class "B" not found
