--TEST--
JIT INC: 015
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $x = 1.0;
    $x += 0;
    return ++$x; // mem -> reg, reg
}
var_dump(foo());
?>
--EXPECT--
float(2)
