--TEST--
php-dev@php.gr.jp #884 (2)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
var_dump(mb_ereg_replace("C?$", "Z", "ABC"));
var_dump(preg_replace("/C?$/", "Z", "ABC"));
var_dump(mb_ereg_replace("C*$", "Z", "ABC"));
var_dump(preg_replace("/C*$/", "Z", "ABC"));
?>
--EXPECT--
string(4) "ABZZ"
string(4) "ABZZ"
string(4) "ABZZ"
string(4) "ABZZ"
