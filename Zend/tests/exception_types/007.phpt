--TEST--
Testing exception type compiler fail inheritance changes throws
--FILE--
<?php
class A {
	public function throwing () throws Exception {
		
	}
}

class B extends A {
	public function throwing() throws FancyException {
	
	}
}
?>
--EXPECTF--
Fatal error: Declaration of B::throwing() throws FancyException must be compatible with A::throwing() throws Exception in %s on line 12



