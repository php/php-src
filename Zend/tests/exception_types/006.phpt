--TEST--
Testing exception type compiler pass inheritance add throws
--FILE--
<?php
class A {
	public function throwing () {
		
	}
}

class B extends A {
	public function throwing() throws Exception {
	
	}
}
?>
OK
--EXPECT--
OK


