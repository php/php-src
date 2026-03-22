--TEST--
Bug #77514: mb_ereg_replace() with trailing backslash adds null byte
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg_replace')) die('skip mb_ereg_replace() not available');
?>
--FILE--
<?php

$a="abc123";
var_dump(mb_ereg_replace("123","def\\",$a));

?>
--EXPECTF--
Deprecated: Function mb_ereg_replace() is deprecated since 8.6, Oniguruma functions support ends PHP 9.0 in %s on line %d
string(7) "abcdef\"
