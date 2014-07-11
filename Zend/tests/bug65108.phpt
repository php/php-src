--TEST--
Bug #65108 (is_callable() triggers Fatal Error)
--FILE--
<?php
class C {
	private function f() {}
	static function __callStatic($name, $args) {}
}

class B {
	public function B() {
		$isCallable = is_callable(array(new C, 'f'));
		var_dump($isCallable);
	}
}

new B();

Class E {
   private function f() {}
   function __call($name, $args) {}
}
$isCallable = is_callable(array('E', 'f'));
var_dump($isCallable);
--EXPECT--
bool(false)
bool(false)
