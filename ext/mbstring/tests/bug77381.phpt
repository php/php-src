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
--EXPECTF--
Warning: mb_ereg(): mbregex compile err: invalid code point value in %sbug77381.php on line %d
bool(false)

Warning: mb_ereg(): mbregex compile err: invalid code point value in %sbug77381.php on line %d
bool(false)

Warning: mb_ereg(): mbregex compile err: invalid code point value in %sbug77381.php on line %d
bool(false)

Warning: mb_ereg(): mbregex compile err: invalid code point value in %sbug77381.php on line %d
bool(false)
