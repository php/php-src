--TEST--
GH-16770 (Tracing JIT type mismatch when returning UNDEF)
--INI--
opcache.jit=1254
opcache.jit_hot_loop=1
opcache.jit_buffer_size=32M
--EXTENSIONS--
opcache
--FILE--
<?php
function ret_undef($k) {
    return $undefined;
}
for ($i = 0; $i < 10; $i++) {
    $output = ret_undef($i);
}
var_dump($output);
?>
--EXPECTF--
Warning: Undefined variable $undefined in %s on line %d

Warning: Undefined variable $undefined in %s on line %d

Warning: Undefined variable $undefined in %s on line %d

Warning: Undefined variable $undefined in %s on line %d

Warning: Undefined variable $undefined in %s on line %d

Warning: Undefined variable $undefined in %s on line %d

Warning: Undefined variable $undefined in %s on line %d

Warning: Undefined variable $undefined in %s on line %d

Warning: Undefined variable $undefined in %s on line %d

Warning: Undefined variable $undefined in %s on line %d
NULL
