--TEST--
Use instead to solve a conflict and as to access the method.
--FILE--
<?php
error_reporting(E_ALL);

trait Hello {
   public function saySomething() {
     echo 'Hello';
   }
}

trait World {
   public function saySomething() {
     echo ' World';
   }
}

class MyHelloWorld {
   use Hello, World {
     Hello::saySomething insteadof World;
     World::saySomething as sayWorld;
   }
}

$o = new MyHelloWorld();
$o->saySomething();
$o->sayWorld();
?>
--EXPECT--
Hello World
