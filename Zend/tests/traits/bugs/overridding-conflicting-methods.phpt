--TEST--
Overriding Conflicting Methods should not result in a notice/warning about collisions
--FILE--
<?php
error_reporting(E_ALL);

trait THello1 {
  public function hello() {
    echo 'Hello';
  }
}

trait THello2 {
  public function hello() {
    echo 'Hello';
  }
}

class TraitsTest {
  use THello1;
  use THello2;
  public function hello() {
    echo 'Hello';
  }
}

$test = new TraitsTest();
$test->hello();
?>
--EXPECTF--	
Hello