--TEST--
ZE2 factory and singleton, test 3
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class test {

  protected function __construct($x) {
  }
}

$obj = new test;

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to protected test::__construct() from invalid context in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
