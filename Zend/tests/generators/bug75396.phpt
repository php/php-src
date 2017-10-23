--TEST--
Bug #75396 (Exit inside generator finally results in fatal error)
--FILE--
<?php

$gen = (function () {

    yield 42;

    try {
    } finally {
        exit;
    }
})();

$gen->send("x");

?>
--EXPECT--