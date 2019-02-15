--TEST--
Trait method overriddes base class method
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
     echo 'World!';
   }
}

class MyHelloWorld extends Base {
   use SayWorld;
}

$o = new MyHelloWorld();
$o->sayHello();
?>
--EXPECT--
World!
