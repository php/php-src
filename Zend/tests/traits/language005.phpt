--TEST--
Use instead to solve a conflict and as to access the method.
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
    use A, B {
		B::smallTalk insteadof A;
		A::bigTalk insteadof B;
		B::bigTalk as talk;
	}
}

$t = new Talker;
$t->smallTalk();
$t->bigTalk();
$t->talk();

?>
--EXPECT--
bAB
