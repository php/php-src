--TEST--
ZE2 factory and singleton, test 8
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class test {

  private function __clone() {
  }
}

$obj = new test;
$clone = clone $obj;
$obj = NULL;

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private test::__clone() from context '' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
