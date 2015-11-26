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
   use HelloWorld { sayHello as protected; }
}


$o = new MyClass;
$o->sayHello();

?>
--EXPECTF--	
Fatal error: Uncaught Error: Call to protected method MyClass::sayHello() from context '' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d