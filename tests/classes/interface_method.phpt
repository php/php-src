--TEST--
ZE2 An interface method must be abstract
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

interface if_a {
	function err() {};
}

?>
--EXPECTF--

Fatal error: Interface function if_a::err() cannot contain body %s on line %d
