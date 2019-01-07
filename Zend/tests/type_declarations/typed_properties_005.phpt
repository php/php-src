--TEST--
Test typed properties error condition (type mismatch object)
--FILE--
<?php
class Dummy {}

new class(new Dummy) {
	public stdClass $std;

	public function __construct(Dummy $dummy) {
		$this->std = $dummy;
	}
};
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property class@anonymous::$std must be an instance of stdClass, Dummy used in %s:8
Stack trace:
#0 %s(4): class@anonymous->__construct(Object(Dummy))
#1 {main}
  thrown in %s on line 8
