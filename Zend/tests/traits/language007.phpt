--TEST--
Traits can fulfill the requirements of abstract base classes.
--FILE--
<?php
error_reporting(E_ALL);

abstract class Base {
  abstract function sayWorld();
}

trait Hello {
   public function sayHello() {
     echo 'Hello';
   }
   public function sayWorld() {
     echo ' World!';
   }
 }

class MyHelloWorld extends Base {
	use Hello;
}

$o = new MyHelloWorld();
$o->sayHello();
$o->sayWorld();

?>
--EXPECTF--	
Hello World!