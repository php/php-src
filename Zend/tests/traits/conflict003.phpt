--TEST--
Two methods resulting in a conflict, should be reported both.
--FILE--
<?php
error_reporting(E_ALL);

trait A {
   public function smallTalk() {
     echo 'a';
   }
   public function bigTalk() {
     echo 'A';
   }
}

trait B {
   public function smallTalk() {
     echo 'b';
   }
   public function bigTalk() {
     echo 'B';
   }
}

class Talker {
   use A, B;
}

?>
--EXPECTF--	
Fatal error: Trait method smallTalk has not been applied, because there are collisions with other trait methods on Talker in %s on line %d