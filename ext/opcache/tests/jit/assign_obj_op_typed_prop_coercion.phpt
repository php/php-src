--TEST--
JIT: compound assignment to a bool property coerces the result
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=1205
--EXTENSIONS--
opcache
--FILE--
<?php
class B {
    public bool $b = false;
}

function f(B $o) {
    $r = ($o->b += 1);
    return $r . "";
}

var_dump(f(new B));
?>
--EXPECT--
string(1) "1"
