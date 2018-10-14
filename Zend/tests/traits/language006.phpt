--TEST--
Express requirements of a trait by abstract methods.
--FILE--
<?php
error_reporting(E_ALL);

trait Hello {
   public function sayHelloWorld() {
     echo 'Hello'.$this->getWorld();
   }
   abstract public function getWorld();
 }

class MyHelloWorld {
   private $world;
   use Hello;
   public function getWorld() {
     return $this->world;
   }
   public function setWorld($val) {
     $this->world = $val;
   }
}

$o = new MyHelloWorld();
$o->setWorld(' World!');
$o->sayHelloWorld();

?>
--EXPECTF--
Hello World!
