--TEST--
JIT FETCH_DIM_R: 016
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
[4][-1];
?>
DONE
--EXPECTF--
Warning: Undefined array key -1 in %sfetch_dim_r_016.php on line 2
DONE
