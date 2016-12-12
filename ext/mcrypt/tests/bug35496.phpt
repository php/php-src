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
Deprecated: Function mcrypt_module_open() is deprecated in %s%ebug35496.php on line 2

Deprecated: Function mcrypt_generic() is deprecated in %s%ebug35496.php on line 3

Warning: mcrypt_generic(): Operation disallowed prior to mcrypt_generic_init(). in %sbug35496.php on line 3

Deprecated: Function mdecrypt_generic() is deprecated in %s%ebug35496.php on line 4

Warning: mdecrypt_generic(): Operation disallowed prior to mcrypt_generic_init(). in %sbug35496.php on line 4
