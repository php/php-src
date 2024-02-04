--TEST--
JIT FETCH_DIM_R: 015
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
array(1, "" => 4)[-1];
?>
DONE
--EXPECTF--
Warning: Undefined array key -1 in %sfetch_dim_r_015.php on line 2
DONE
