--TEST--
Bug #35496 (Crash in mcrypt_generic()/mdecrypt_generic() without proper init).
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$td = mcrypt_module_open('rijndael-256', '', 'ofb', '');
mcrypt_generic($td, "foobar");
mdecrypt_generic($td, "baz");
?>
--EXPECTF--
Warning: mcrypt_generic(): Operation disallowed prior to mcrypt_generic_init(). in %sbug35496.php on line 3

Warning: mdecrypt_generic(): Operation disallowed prior to mcrypt_generic_init(). in %sbug35496.php on line 4
