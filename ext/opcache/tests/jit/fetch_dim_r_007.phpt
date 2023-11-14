--TEST--
JIT FETCH_DIM_R: 007
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function &test() {
    $var = [0];
    return $var;
}
var_dump(test()[0]);
?>
--EXPECT--
int(0)
