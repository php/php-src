--TEST--
ZE2 A method may be redeclared final
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class first {
	function show() {
		echo "Call to function first::show()\n";
	}
}

$t = new first();
$t->show();

class second extends first {
	final function show() {
		echo "Call to function second::show()\n";
	}
}

$t2 = new second();
$t2->show();

echo "Done\n";
?>
--EXPECTF--
Call to function first::show()
Call to function second::show()
Done