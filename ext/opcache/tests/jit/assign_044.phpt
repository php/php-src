--TEST--
JIT ASSIGN: DASM_S_RANGE_VREG error
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
function test($x) {
    if ($x) {
        unknown($value);
    }
    var_dump($value = INF);
}
test(false);
?>
--EXPECT--
float(INF)

