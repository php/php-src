--TEST--
Test crypt_preferred_method
--SKIPIF--
<?php if (!function_exists('crypt_preferred_method')) die('skip needs external libxcrypt >= 4.4'); ?>
--FILE--
<?php
var_dump(crypt_preferred_method());
?>
--EXPECTF--
string(%d) "$%s$"

