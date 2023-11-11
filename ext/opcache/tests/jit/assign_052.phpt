--TEST--
JIT ASSIGN: incorrect reference counting
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--FILE--
<?php
function foo(){
    for($cnt = 0; $cnt < 6; $cnt++) {
        $t[$i = $s][] = [] > $n[$i = $j] = $s = $a . $a = $f;
    }
}
@foo();
?>
DONE
--EXPECT--
DONE
