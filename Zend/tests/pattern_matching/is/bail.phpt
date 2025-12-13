--TEST--
Object pattern matching
--FILE--
<?php

(function () {
    $o = new stdClass();

    try {
        var_dump($o is self);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        var_dump($o is parent);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        var_dump($o is static);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
})();

?>
--EXPECT--
Error: Cannot access "self" when no class scope is active
Error: Cannot access "parent" when no class scope is active
Error: Cannot access "static" when no class scope is active
