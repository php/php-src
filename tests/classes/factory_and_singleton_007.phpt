--TEST--
ZE2 factory and singleton, test 7
--FILE--
<?php
class test {

  protected function __clone() {
  }
}

try {
  $obj = new test;
  $clone = clone $obj;
} catch (Throwable $e) {
  echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Call to protected method test::__clone() from global scope
