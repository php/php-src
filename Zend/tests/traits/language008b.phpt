--TEST--
Visibility can be changed with the as aliasing construct as well.
--FILE--
<?php
error_reporting(E_ALL);

trait HelloWorld {
   public function sayHello() {
     echo 'Hello World!';
   }
}

class MyClass {
   use HelloWorld { sayHello as private sayHelloWorld; }

   public function callPrivateAlias() {
      $this->sayHelloWorld();
   }
}

$o = new MyClass();
$o->sayHello();
$o->callPrivateAlias();
$o->sayHelloWorld();


?>
--EXPECTF--	
Hello World!Hello World!
Fatal error: Call to private method MyClass::sayHelloWorld() from context '' in %s on line %d