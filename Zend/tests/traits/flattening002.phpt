--TEST--
parent:: works like in a method defined without traits. 
--FILE--
<?php
error_reporting(E_ALL);

class Base {
   public function sayHello() {
     echo 'Hello ';
   }
}
 
trait SayWorld {
   public function sayHello() {
     parent::sayHello();
     echo 'World!';
   }
}

class MyHelloWorld extends Base {
   use SayWorld;
}

$o = new MyHelloWorld();
$o->sayHello();
?>
--EXPECTF--	
Hello World!