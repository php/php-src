--TEST--
JIT FETCH_DIM_W: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function &foo() {
    $a = array(1);
    return $a[-1];
}

var_dump(foo());
?>
--EXPECT--
NULL
