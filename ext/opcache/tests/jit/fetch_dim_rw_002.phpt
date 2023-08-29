--TEST--
JIT FETCH_DIM_RW: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=0
--FILE--
<?php
$a = [];
$k = "0";
$a[$k]++;
var_dump($a[$k]);
?>
--EXPECTF--
Warning: Undefined array key 0 in %sfetch_dim_rw_002.php on line 4
int(1)
