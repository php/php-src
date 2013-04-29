--TEST--
ZE2 factory and singleton, test 5
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class test {

  protected function __destruct() {
  }
}

$obj = new test;
$obj = NULL;

echo "Done\n";
?>
--EXPECTF--
Fatal error: Call to protected test::__destruct() from context '' in %sfactory_and_singleton_005.php on line %d
