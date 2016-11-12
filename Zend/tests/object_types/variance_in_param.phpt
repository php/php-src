--TEST--
Variance of object parameter type
--FILE--
<?php
class Foo {
   public function qux(object $object) {
       
   }
}

class Bar extends Foo {
   public function qux(Qux $qux) {}
}

var_dump(new Bar);
?>
--EXPECTF--
object(Bar)#%d (0) {
}

