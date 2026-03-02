--TEST--
Aliasing on conflicting method should not cover up conflict.
--FILE--
<?php
error_reporting(E_ALL);

trait Hello {
   public function sayHello() {
     echo 'Hello';
   }
}

trait World {
   public function sayHello() {
     echo ' World!';
   }
}


class MyClass {
   use Hello, World { World::sayHello as sayWorld; }
}

$o = new MyClass();
$o->sayHello();
$o->sayWorld();

?>
--EXPECTF--
Fatal error: Trait method World::sayHello has not been applied as MyClass::sayHello, because of collision with Hello::sayHello in %s on line %d
