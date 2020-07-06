--TEST--
ZE2 factory and singleton, test 8
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
Fatal error: Uncaught Error: Private method test::__clone() cannot be called from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
