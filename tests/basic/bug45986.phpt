--TEST--
Bug #45986 (wrong error message for a non existent file on rename)
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php
rename('foo', 'bar');
?>
--EXPECTF--
Warning: %s in %sbug45986.php on line 2
