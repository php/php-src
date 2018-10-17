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
Fatal error: Trait method hello has not been applied, because there are collisions with other trait methods on TraitsTest in %s on line %d
