--TEST--
Bug #72710 (`mb_ereg` causes buffer overflow on regexp compile error)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
mb_ereg('(?<0>a)', 'a');
?>
--EXPECTF--
Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Warning: mb_ereg(): mbregex compile err: invalid group name <0> in %s on line %d
