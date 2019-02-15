--TEST--
Trait method overwridden by a method defined in the class.
--FILE--
<?php
error_reporting(E_ALL);

trait HelloWorld {
   public function sayHello() {
     echo 'Hello World!';
   }
}

class TheWorldIsNotEnough {
   use HelloWorld;
   public function sayHello() {
     echo 'Hello Universe!';
   }
}

$o = new TheWorldIsNotEnough();
$o->sayHello(); // echos Hello Universe!
?>
--EXPECT--
Hello Universe!
