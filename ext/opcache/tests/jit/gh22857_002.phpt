--TEST--
GH-22857 (reg-alloc): FETCH_OBJ_FUNC_ARG hook read must not lose register-held vars
--ENV--
A=1
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=1205
opcache.jit_hot_func=1
--FILE--
<?php

class C {
    public $prop {
        get { return 1; }
    }
}

function f(int $a0, $obj) {
    // $a lives only in a register: every use is in a supported opcode and in a
    // non-entry basic block, so the register allocator never spills it to the
    // VM stack. Exiting to the VM (the buggy hook-enter guard) would read a
    // stale stack slot for $a.
    $a = $a0;
    $b = $a + 2;
    $c = g($obj->prop, $a ? 1 : 2);
    return $b + $c;
}

if (getenv('A')) {
    function g($a, $b) {
        var_dump($b);
        return $a;
    }
}

$c = new C();
var_dump(f(1, $c));
var_dump(f(1, $c));

?>
--EXPECT--
int(1)
int(4)
int(1)
int(4)
