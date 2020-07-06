--TEST--
ZE2 factory and singleton, test 6
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
Fatal error: Uncaught Error: Private method test::__destruct() cannot be called from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %sfactory_and_singleton_006.php on line %d
