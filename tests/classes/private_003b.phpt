--TEST--
ZE2 A private method cannot be called in a derived class
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class pass {
	private function show() {
		echo "Call show()\n";
	}

	protected function good() {
		$this->show();
	}
}

class fail extends pass {
	public function ok() {
		$this->good();
	}

	public function not_ok() {
		$this->show();
	}
}

$t = new fail();
$t->ok();
$t->not_ok(); // calling a private function

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call show()

Fatal error: Call to private method pass::show() from context 'fail' in %s on line %d
