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
Fatal error: Uncaught Error: Call to private test::__destruct() from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %sfactory_and_singleton_006.php on line %d
