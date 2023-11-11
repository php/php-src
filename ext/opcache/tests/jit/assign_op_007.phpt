--TEST--
JIT ASSIGN_OP: 007 Arrays merging with return value 
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function test() {
	$a = [];
    for ($i=0; $i < 2; $i++) {
        $a += $a + $a += $a;
        $a['b'] += 1;
    }
}
test();
?>
DONE
--EXPECTF--
Warning: Undefined array key "b" in %sassign_op_007.php on line 6
DONE
