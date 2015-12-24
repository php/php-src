--TEST--
ZE2 type
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class T {
	function f(P $p = 42) {
	}
}
?>
--EXPECTF--

Fatal error: Default value for parameters with a class type can only be NULL in %stype_hints_003.php on line 3
