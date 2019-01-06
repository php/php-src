--TEST--
Bug #77370 (Buffer overflow on mb regex functions - fetch_token)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(mb_split("   \xfd",""));
?>
--EXPECTF--
Warning: mb_split(): mbregex compile err: invalid code point value in %sbug77370.php on line %d
bool(false)
