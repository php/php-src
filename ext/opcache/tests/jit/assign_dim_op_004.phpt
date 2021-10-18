--TEST--
JIT ASSIGN_DIM_OP: Cyclic dependency
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
$a = null; 
$a[] .= $a;
var_dump($a);
?>
--EXPECTF--
Warning: Array to string conversion in %sassign_dim_op_004.php on line 3
array(1) {
  [0]=>
  string(5) "Array"
}

