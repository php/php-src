--TEST--
ZE2 An interface method cannot be final
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class if_a {
	abstract final function err();
}

?>
--EXPECTF--

Fatal error: Cannot use the final modifier on an abstract class member in %s on line %d
