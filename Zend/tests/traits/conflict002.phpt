--TEST--
Overwridden methods do not cause a conflict.
--FILE--
<?php
error_reporting(E_ALL);

trait HelloWorld {
   public function sayHello() {
     echo 'Hello World!';
   }
}

trait HelloWorld2 {
   public function sayHello() {
     echo 'Hello World2!';
   }
}


class TheWorldIsNotEnough {
   use HelloWorld;
   use HelloWorld2;
   public function sayHello() {
     echo 'Hello Universe!';
   }
}

$o = new TheWorldIsNotEnough();
$o->sayHello(); // echos Hello Universe!
?>
--EXPECTF--	
Hello Universe!