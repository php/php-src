--TEST--
GH-24398 (tracing): FETCH_OBJ_FUNC_ARG of a hooked property with unknown send mode
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=tracing
opcache.jit_hot_loop=1
--EXTENSIONS--
opcache
--FILE--
<?php

class A {
    public $prop = 5;
}

class B {
    public $prop {
        get => 5;
    }
}

if (true) {
    function g($x) {
        return $x;
    }
}

function f($o) {
    // The named argument leaves the send mode unknown to the tracing JIT,
    // so FETCH_OBJ_FUNC_ARG is executed by the VM handler.
    return g(x: $o->prop);
}

$sum = 0;
foreach ([new A, new A, new A, new B, new B, new B] as $o) {
    $sum += f($o);
}
var_dump($sum);

?>
--EXPECT--
int(30)
