--TEST--
Aliasing leading to conflict should result in error message
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
   use Hello, World { sayHello as sayWorld; }
}

$o = new MyClass();
$o->sayHello();
$o->sayWorld();

?>
--EXPECTF--	
Warning: Trait method sayWorld has not been applied, because there are collisions with other trait methods on MyClass in %s on line %d

Warning: Trait method sayHello has not been applied, because there are collisions with other trait methods on MyClass in %s on line %d

Fatal error: Call to undefined method MyClass::sayHello() in %s on line %d