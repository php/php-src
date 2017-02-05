--TEST--
Testing exception type compiler fail inheritance remove throws
--FILE--
<?php
class A {
	public function throwing () throws Exception {
		
	}
}

class B extends A {
	public function throwing() {
	
	}
}
?>
--EXPECTF--
Fatal error: Declaration of B::throwing() must be compatible with A::throwing() throws Exception in %s on line 12



