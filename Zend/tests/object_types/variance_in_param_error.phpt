--TEST--
Variance of object parameter type (fail)
--FILE--
<?php
class Foo {
   public function qux(Qux $object) {
       
   }
}

class Bar extends Foo {
   public function qux(object $qux) {}
}

var_dump(new Bar);
?>
--EXPECTF--
Warning: Declaration of Bar::qux(object $qux) should be compatible with Foo::qux(Qux $object) in %s on line 10
object(Bar)#%d (0) {
}

