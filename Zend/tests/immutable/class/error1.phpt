--TEST--
Ensure that multiple immutable class modifiers result in a fatal error.
--FILE--
<?php
namespace X\Y\Z;
immutable immutable class A {}
?>
--EXPECTF--

Fatal error: Multiple immutable modifiers are not allowed in %serror1.php on line 3
