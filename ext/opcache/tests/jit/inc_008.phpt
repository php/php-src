--TEST--
JIT INC: 008
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $x = 1;
    return ++$x; // reg -> reg, reg
}
var_dump(foo());
?>
--EXPECT--
int(2)
