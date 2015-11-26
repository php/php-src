--TEST--
ZE2 __set() signature check
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class Test {
	function __set() {
	}
}

?>
--EXPECTF--
Fatal error: Method Test::__set() must take exactly 2 arguments in %s__set__get_003.php on line %d
