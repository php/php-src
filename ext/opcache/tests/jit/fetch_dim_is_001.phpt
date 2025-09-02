--TEST--
JIT FETCH_DIM_IS: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
const A = [1];
A[-1][2]??$y;
?>
DONE
--EXPECTF--
Warning: Undefined variable $y in %sfetch_dim_is_001.php on line 3
DONE
