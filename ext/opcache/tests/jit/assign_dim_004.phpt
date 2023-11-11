--TEST--
JIT ASSIGN_DIM: 004
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
class Test implements ArrayAccess {
    function offsetExists($x): bool {}
    function offsetGet($x): mixed {}
    function offsetSet($x, $y): void {
        echo "offsetSet($x, $y)\n";
    }
    function offsetUnset($x): void {}
}
function test() {
    $obj = new Test;
    $obj[$undef] = 1;
}
test();
?>
--EXPECTF--
Warning: Undefined variable $undef in %s on line %d
offsetSet(, 1)
