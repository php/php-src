--TEST--
JIT BW_NOT: 001 Incorrect refcounting inference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
$x[~"$x"]*=1;
?>
DONE
--EXPECTF--
Warning: Undefined variable $x in %sbw_not_001.php on line 2

Warning: Undefined variable $x in %sbw_not_001.php on line 2

Warning: Undefined array key "" in %sbw_not_001.php on line 2
DONE