--TEST--
Bug #72703 Out of bounds global memory read in BF_crypt triggered by password_verify
--SKIPIF--
<?php
if (!function_exists('crypt'))) {
	die("SKIP crypt() is not available");
}
?> 
--FILE--
<?php
	var_dump(password_verify("","$2y$10$$"));
?>
==OK==
--EXPECT--
bool(false)
==OK==

