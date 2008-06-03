--TEST--
Invalid method name in dynamic static call
--XFAIL--
http://bugs.php.net/bug.php?id=45089
--FILE--
<?php

class foo {
	static function __callstatic($a, $b) {
		var_dump($a);
	}
}

foo::AaA();

$a = 1;
foo::$a();

?>
--EXPECTF--
unicode(3) "AaA"

Fatal error: Function name must be a string in %s on line %d
