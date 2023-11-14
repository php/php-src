--TEST--
JIT ASSIGN: 004
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $a = null;
    $b = $a;
    $c = null;
    $d = $c;
    $a = 1;
    $c = $a;
    return $c;
}
var_dump(foo());
?>
--EXPECT--
int(1)
