--TEST--
An interface must be implemented
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

interface if_a {
	abstract function f_a();
}
	
class derived_a implements if_a {
}

?>
--EXPECTF--
Fatal error: Class derived_a contains abstract methods and must be declared abstract in %s on line %d
