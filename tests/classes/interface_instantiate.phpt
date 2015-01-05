--TEST--
ZE2 An interface cannot be instantiated
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

interface if_a {
	function f_a();
}
	
$t = new if_a();

?>
--EXPECTF--
Fatal error: Cannot instantiate interface if_a in %s on line %d
