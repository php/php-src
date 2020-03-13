--TEST--
Method conflict in traits
--FILE--
<?php
error_reporting(E_ALL);

trait THello1 {
  private function hello() {
    echo 'Hello';
  }
}

trait THello2 {
  private function hello() {
    echo 'Hello';
  }
}

class TraitsTest {
    use THello1;
    use THello2;
}
?>
--EXPECTF--
Fatal error: Trait method THello2::hello has not been applied as TraitsTest::hello, because of collision with THello1::hello in %s on line %d
