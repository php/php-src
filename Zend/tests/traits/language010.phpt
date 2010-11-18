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
   use Hello, World { hello as world; }
}

$o = new MyClass();
$o->hello();
$o->world();

?>
--EXPECTF--	
Fatal error: Failed to add trait method (world) to the trait table. There is probably already a trait method with the same name in %s on line %d