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
Fatal error: Uncaught Error: Call to private test::__clone() from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
