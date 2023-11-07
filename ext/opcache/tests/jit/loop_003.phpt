--TEST--
JIT LOOP: 003 Incorrect dead IR edge elimination
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=32M
--FILE--
<?php
function () {
    $a = 0;
    while (y) {
        $a &= $y & $y;
        if (y) die &("");
     }
};
?>
DONE
--EXPECT--
DONE
