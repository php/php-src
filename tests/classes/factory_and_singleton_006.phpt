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
Warning: Call to private destructor from context '' in %sfactory_and_singleton_006.php on line %d
Done
