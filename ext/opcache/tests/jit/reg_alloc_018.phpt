--TEST--
Register Alloction 018: Incorrect allocation
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
6 & $y & 6 & $y;
?>
DONE
--EXPECTF--
Warning: Undefined variable $y in %sreg_alloc_018.php on line 2

Warning: Undefined variable $y in %sreg_alloc_018.php on line 2
DONE
