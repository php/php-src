--TEST--
JIT INC: 018
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $x = false;
    return ++$x; // reg -> reg, reg
}
var_dump(foo());
?>
--EXPECTF--
Warning: Increment on type bool has no effect, this will change in the next major version of PHP in %sinc_018.php on line 4
bool(false)
