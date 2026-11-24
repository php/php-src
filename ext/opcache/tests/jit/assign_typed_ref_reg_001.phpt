--TEST--
JIT ASSIGN: result must not be register-allocated when op1 is traced as a reference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_loop=1
opcache.jit_hot_side_exit=1
--EXTENSIONS--
opcache
--FILE--
<?php
function test() {
    for ($i = 0; $i < 100; $i++) {
        +$a = +$a .= $a + $a .= +$x;
        $x = &$a + $x++;
    }
}
try {
    test();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: Undefined variable $x in %s on line %d

Warning: Undefined variable $a in %s on line %d
%A
TypeError: Unsupported operand types: string + string
