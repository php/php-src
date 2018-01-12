--TEST--
Bug #75396: Exit inside generator finally results in fatal error
--FILE--
<?php

$gen = (function () {
    yield 42;

    try {
        echo "Try\n";
        exit("Exit\n");
    } finally {
        echo "Finally\n";
    }
})();

$gen->send("x");

?>
--EXPECT--
Try
Exit
