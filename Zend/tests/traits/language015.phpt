--TEST--
Invalid conflict resolution (unused trait as lhs of "insteadof")
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
		T2::foo insteadof T1;
	}
}
--EXPECTF--
Fatal error: Required Trait T2 wasn't added to C in %slanguage015.php on line %d
