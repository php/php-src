--TEST--
Variance of object return type
--FILE--
<?php
class Foo {
   public function qux() : object {
       
   }
}

class Bar extends Foo {
   public function qux() : Qux {}
}

var_dump(new Bar);
?>
--EXPECTF--
object(Bar)#%d (0) {
}

