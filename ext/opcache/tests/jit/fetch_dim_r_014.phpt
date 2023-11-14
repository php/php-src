--TEST--
JIT FETCH_DIM_R: 014
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function foo() {
    $s =+ $y;
    $tokenthiss[$i=$s][] = $y = $y;
    $tokenthiss[$i][$i] + $y;
}
@foo();
?>
DONE
--EXPECT--
DONE
