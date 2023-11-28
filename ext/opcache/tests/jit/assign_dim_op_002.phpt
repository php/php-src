--TEST--
JIT ASSIGN_DIM_OP: Undefined variable
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
$a[] &= 1;
?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d
