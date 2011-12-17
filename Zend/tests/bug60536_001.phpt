--TEST--
Bug #60536 (Traits Segfault)
--FILE--
<?php
trait T { private $x = 0; }
class X { 
	use T;
}
class Y extends X {
	  use T;
	  function x() {
	      return ++$this->x; 
      }
}
class Z extends Y {
	  function z() {
		  return ++$this->x; 
      }
}
$a = new Z();
$a->x();
echo "DONE";
?>
--EXPECTF--
DONE
