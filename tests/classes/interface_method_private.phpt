--TEST--
ZE2 An interface method cannot be private
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

interface if_a {
	abstract private function err();
}

?>
--EXPECTF--

Fatal error: Access type for interface method if_a::err() must be omitted in %s on line %d
