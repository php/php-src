--TEST--
A private method can be overwritten in a second derived class
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class first {
	private static function show() {
		echo "Call show()\n";
	}

	public static function do_show() {
		first::show();
	}
}

first::do_show();

class second extends first { 
}

second::do_show();

class third extends second {
}

third::do_show();

class fail extends third {
	static function show() {  // cannot be redeclared
		echo "Call show()\n";
	}
}

echo "Done\n";
?>
--EXPECTF--
Call show()
Call show()
Call show()
Done
