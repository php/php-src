--TEST--
ZE2 A redeclared method must have the same or higher visibility
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class father {
	function f0() {}
	function f1() {}
	public function f2() {}
	protected function f3() {}
	private function f4() {}
}

class same extends father {

	// overload fn with same visibility
	function f0() {}
	public function f1() {}
	public function f2() {}
	protected function f3() {}
	private function f4() {}
}

class fail extends same {
	protected function f1() {}
}

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Fatal error: Access level to fail::f1() must be public (as in class same) in %s on line %d
