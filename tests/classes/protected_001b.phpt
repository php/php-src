--TEST--
A protected method can only be called inside the class
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class pass {
	protected function fail() {
		echo "Call fail()\n";
	}

	public function good() {
		$this->fail();
	}
}

$t = new pass();
$t->good();
$t->fail();// must fail because we are calling from outside of class pass

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call fail()

Fatal error: Call to protected method pass::fail() from context '' in %s on line %d
