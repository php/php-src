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
--EXPECT--
string(7) "abcdef\"
