--TEST--
ZE2 factory and singleton, test 6
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class test {

  private function __destruct() {
  }
}

$obj = new test;
$obj = NULL;

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private test::__destruct() from context '' in %sfactory_and_singleton_006.php:%d
Stack trace:
#0 {main}
  thrown in %sfactory_and_singleton_006.php on line %d

