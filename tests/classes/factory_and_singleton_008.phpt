--TEST--
ZE2 factory and singleton, test 8
--FILE--
<?php
class test {

  private function __clone() {
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
Error: Call to private method test::__clone() from global scope
