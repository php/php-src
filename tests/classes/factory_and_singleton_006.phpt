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
Done

Fatal error: Uncaught Error: Call to private test::__destruct() from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
