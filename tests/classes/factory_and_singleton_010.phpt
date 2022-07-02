--TEST--
ZE2 factory and singleton, test 10
--FILE--
<?php
class test {

  private function __destruct() {
    echo __METHOD__ . "\n";
  }
}

$obj = new test;

?>
===DONE===
--EXPECT--
===DONE===

Warning: Call to private test::__destruct() from global scope during shutdown ignored in Unknown on line 0
