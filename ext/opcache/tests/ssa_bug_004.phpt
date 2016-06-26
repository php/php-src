--TEST--
Assign elision exception safety: ICALL
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
