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
Fatal error: Trait method B::smallTalk has not been applied as Talker::smallTalk, because of collision with A::smallTalk in %s on line %d
