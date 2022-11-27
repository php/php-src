--TEST--
Object pattern matching
--FILE--
<?php

(function () {
    $o = new stdClass();

    try {
        var_dump($o is self);
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        var_dump($o is parent);
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        var_dump($o is static);
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }
})();

?>
--EXPECT--
Cannot access "self" when no class scope is active
Cannot access "parent" when no class scope is active
Cannot access "static" when no class scope is active
