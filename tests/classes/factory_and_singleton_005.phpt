--TEST--
ZE2 factory and singleton, test 5
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
Fatal error: Uncaught Error: Call to protected test::__destruct() from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %sfactory_and_singleton_005.php on line %d
