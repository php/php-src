--TEST--
Variance of object return type (fail)
--FILE--
<?php
class Foo {
   public function qux() : Qux {
       
   }
}

class Bar extends Foo {
   public function qux() : object {}
}

var_dump(new Bar);
?>
--EXPECTF--
Fatal error: Declaration of Bar::qux(): object must be compatible with Foo::qux(): Qux in %s on line 10


