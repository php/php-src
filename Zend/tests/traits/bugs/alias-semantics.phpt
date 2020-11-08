--TEST--
Semantic of alias operation is to provide an additional identifier for the method body of the original method.
--FILE--
<?php
error_reporting(E_ALL);

trait THello {
  public function a() {
    echo 'A';
  }
}

class TraitsTest {
    use THello { a as b; }
}

$test = new TraitsTest();
$test->a();
$test->b();

?>
--EXPECT--
AA
