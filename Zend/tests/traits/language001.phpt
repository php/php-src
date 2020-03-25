--TEST--
Single Trait with simple trait method
--FILE--
<?php
error_reporting(E_ALL);

trait THello {
  public function hello() {
    echo 'Hello';
  }
}

class TraitsTest {
    use THello;
}

$test = new TraitsTest();
$test->hello();
?>
--EXPECT--
Hello
