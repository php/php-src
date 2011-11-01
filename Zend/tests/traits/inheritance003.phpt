--TEST--
Trait method overrides base class method and satisfies prototype
--FILE--
<?php
error_reporting(E_ALL);

abstract class Base {
   public abstract function sayHello(array $a);
}

class SubClass extends Base {
   public function sayHello(array $a) {
     echo "World!\n";
   }
}

$s = new SubClass();
$s->sayHello(array());


trait SayWorld {
   public function sayHello(Base $d) {
     echo 'World!';
   }
}

class MyHelloWorld extends Base {
   use SayWorld;
}

$o = new MyHelloWorld();
$o->sayHello(array());

?>
--EXPECTF--	
World!

Fatal error: Declaration of MyHelloWorld::sayHello() must be compatible with Base::sayHello(array $a) in %s on line %d
