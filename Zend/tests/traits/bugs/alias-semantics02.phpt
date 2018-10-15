--TEST--
Semantic of alias operation is to provide an additional identifier for the
method body of the original method.
It should also work incase the method is fully qualified.
--FILE--
<?php
error_reporting(E_ALL);

trait THello {
  public function a() {
    echo 'A';
  }
}

class TraitsTest {
	use THello { THello::a as b; }
}

$test = new TraitsTest();
$test->a();
$test->b();

?>
--EXPECT--
AA
