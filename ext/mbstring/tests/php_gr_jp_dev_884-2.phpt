--TEST--
php-dev@php.gr.jp #884 (2)
--FILE--
<?php
var_dump(mb_ereg_replace("C?$", "Z", "ABC"));
var_dump(ereg_replace("C?$", "Z", "ABC"));
var_dump(mb_ereg_replace("C*$", "Z", "ABC"));
var_dump(ereg_replace("C*$", "Z", "ABC"));
?>
--EXPECT--
string(4) "ABZZ"
string(4) "ABZZ"
string(4) "ABZZ"
string(4) "ABZZ"
