--TEST--
Register Alloction 004: Check guard before register load
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
opcache.jit=tracing
opcache.jit_hot_func=1
--EXTENSIONS--
opcache
--FILE--
<?php
function createTree($depth) {
    if (!$depth) {
        return;
    }
    $depth--;
    [createTree($d), createTree($depth)]();
}
createTree(4);
?>
--EXPECTF--
Warning: Undefined variable $d in %sreg_alloc_004.php on line 7

Warning: Undefined variable $d in %sreg_alloc_004.php on line 7

Warning: Undefined variable $d in %sreg_alloc_004.php on line 7

Warning: Undefined variable $d in %sreg_alloc_004.php on line 7

Fatal error: Uncaught Error: First array member is not a valid class name or object in %sreg_alloc_004.php:7
Stack trace:
#0 %sreg_alloc_004.php(7): createTree(0)
#1 %sreg_alloc_004.php(7): createTree(1)
#2 %sreg_alloc_004.php(7): createTree(2)
#3 %sreg_alloc_004.php(9): createTree(3)
#4 {main}
  thrown in %sreg_alloc_004.php on line 7
