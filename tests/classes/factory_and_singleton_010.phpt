--TEST--
ZE2 factory and singleton, test 10
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class test {

  private function __destruct() {
  	echo __METHOD__ . "\n";
  }
}

$obj = new test;

?>
===DONE===
--EXPECTF--
===DONE===

Warning: Call to private test::__destruct() from context '' during shutdown ignored in Unknown on line 0
