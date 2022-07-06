--TEST--
ZE2 factory and singleton, test 9
--FILE--
<?php
class test {

  protected function __destruct() {
    echo __METHOD__ . "\n";
  }
}

$obj = new test;

?>
===DONE===
--EXPECT--
===DONE===

Warning: Call to protected test::__destruct() from global scope during shutdown ignored in Unknown on line 0
