--TEST--
ZE2 An interface cannot have properties
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

interface if_a {
	public $member;
}
?>
--EXPECTF--
Fatal error: Interfaces may not include member variables in %s on line %d
