--TEST--
Aliasing leading to conflict should result in error message
--FILE--
<?php
error_reporting(E_ALL);

trait Hello {
   public function hello() {
     echo 'Hello';
   }
}

trait World {
   public function world() {
     echo ' World!';
   }
}


class MyClass {
   use Hello, World { world as hello; }
}

$o = new MyClass();
$o->hello();
$o->world();

?>
--EXPECTF--	
Fatal error: Trait method hello has not been applied, because there are collisions with other trait methods on MyClass in %s on line %d
