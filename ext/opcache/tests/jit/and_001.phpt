--TEST--
JIT BW_AND: 001 (empty string)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
$a = [];
$b = "";
$a["" & $b] *= 3;
?>
DONE
--EXPECTF--
Warning: Undefined array key "" in %sand_001.php on line 4
DONE
