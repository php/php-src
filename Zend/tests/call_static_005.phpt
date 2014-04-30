--TEST--
Empty method name in dynamic static call
--FILE--
<?php

class foo {
	static function __callstatic($a, $b) {
		var_dump($a);
	}
}

$a = 'foo::';
$a();

$a = array('foo', '');
$a();

?>
--EXPECTF--
string(0) ""
string(0) ""

