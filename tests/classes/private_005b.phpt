--TEST--
A private method cannot be called in a derived class
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class pass {
	private function show() {
		echo "Call show()\n";
	}

	public function do_show() {
		$this->show();
	}
}

class fail extends pass {
	function do_show() {
		$this->show();
	}
}

$t = new pass();
$t->do_show();

$t2 = new fail();
$t2->do_show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call show()

Fatal error: Call to private method pass::show() from context 'fail' in %s on line %d