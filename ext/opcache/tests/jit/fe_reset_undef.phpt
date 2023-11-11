--TEST--
FE_RESET with potentially undef operand
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function test($c) {
    if ($c) {
        $a[] = null;
    }
    foreach ($a as $k) {}
}
test(false);
?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d

Warning: foreach() argument must be of type array|object, null given in %s on line %d
