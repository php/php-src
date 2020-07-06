--TEST--
ZE2 factory and singleton, test 3
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
Fatal error: Uncaught Error: Protected method test::__construct() cannot be called from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
