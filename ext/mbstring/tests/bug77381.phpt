--TEST--
Bug #77381 (heap buffer overflow in multibyte match_at)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(mb_ereg("000||0\xfa","0"));
var_dump(mb_ereg("(?i)000000000000000000000\xf0",""));
var_dump(mb_ereg("0000\\"."\xf5","0"));
var_dump(mb_ereg("(?i)FFF00000000000000000\xfd",""));
?>
--EXPECT--
int(1)
bool(false)
bool(false)
bool(false)
