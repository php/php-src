--TEST--
Invalid conflict resolution (unused trait as rhs of "insteadof")
--FILE--
<?php
trait T1 {
	function foo() {echo "T1\n";}
}
trait T2 {
        function foo() {echo "T2\n";}
}
class C {
	use T1 {
		T1::foo insteadof T2;
	}
}
--EXPECTF--
Fatal error: Trait T2 is not used in %s on line %d
