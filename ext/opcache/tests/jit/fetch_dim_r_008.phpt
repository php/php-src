--TEST--
JIT FETCH_DIM_R: 008
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function &test() { return $x; }
test()[1];
?>
DONE
--EXPECTF--
Warning: Trying to access array offset on null in %sfetch_dim_r_008.php on line 3
DONE
