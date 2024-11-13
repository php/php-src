--TEST--
JIT INC: 012
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
    $x = 1.0;
    ++$x; // reg -> reg
    return $x;
}
var_dump(foo());
?>
--EXPECT--
float(2)
