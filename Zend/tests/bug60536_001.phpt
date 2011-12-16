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
Strict Standards: X and T define the same property ($x) in the composition of Y. This might be incompatible, to improve maintainability consider using accessor methods in traits instead. Class was composed in %sbug60536_001.php on line %d
DONE
