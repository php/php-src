--TEST--
GH-24398: Function JIT level 1 crash on FETCH_OBJ_FUNC_ARG of a hooked property
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=1201
--EXTENSIONS--
opcache
--FILE--
<?php

class C {
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
    return g($o->prop);
}

$c = new C();
var_dump(f($c));
var_dump(f($c));

?>
--EXPECT--
int(5)
int(5)
