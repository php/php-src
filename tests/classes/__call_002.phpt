--TEST--
ZE2 __call() signature check
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class Test {
	function __call() {
	}
}

?>
--EXPECTF--
Fatal error: Method Test::__call() must take exactly 2 arguments in %s__call_002.php on line %d
