--TEST--
JIT ASSIGN_DIM: 010
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test() {
    for($i=0; $i<10; $i++) {
        $a[] &= $y;
        $a = false;
        $a[] =& $y;
     }
}
test();
?>
DONE
--EXPECTF--
Warning: Undefined variable $a in %sassign_dim_010.php on line 4

Warning: Undefined variable $y in %sassign_dim_010.php on line 4
DONE
