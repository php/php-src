--TEST--
ZE2 Initializing static properties to arrays
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class test {
	static public $ar = array();
}

var_dump(test::$ar);

test::$ar[] = 1;

var_dump(test::$ar);

echo "Done\n";
?>
--EXPECTF--
array(0) {
}
array(1) {
  [0]=>
  int(1)
}
Done
