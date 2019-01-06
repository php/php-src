--TEST--
Bug #77371 (heap buffer overflow in mb regex functions - compile_string_node)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(mb_ereg("()0\xfc00000\xfc00000\xfc00000\xfc",""));
?>
--EXPECTF--
Warning: mb_ereg(): mbregex compile err: invalid code point value in %sbug77371.php on line %d
bool(false)
