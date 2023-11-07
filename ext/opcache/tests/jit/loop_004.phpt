--TEST--
JIT LOOP: 004 Incorrect trace type inference in a false loop
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=32M
--FILE--
<?php
function foo() {
    for($i = 0; $i < 6; $i++) {
        $a ?: $y;
        $a = 3;
        for(;$y;) $a = $a;
    }
}
@foo();
?>
DONE
--EXPECT--
DONE
