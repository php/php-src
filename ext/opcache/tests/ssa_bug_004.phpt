--TEST--
Assign elision exception safety: ICALL
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

set_error_handler(function() { throw new Exception; });

function test() {
    $x = str_replace(['foo'], [[]], ['foo']);
}
try {
    test();
} catch (Exception $e) {
    echo "caught\n";
}

?>
--EXPECT--
caught
