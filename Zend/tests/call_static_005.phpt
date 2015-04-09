--TEST--
Invalid method name in dynamic static call
--FILE--
<?php

class foo {
	static function __callstatic($a, $b) {
		var_dump($a);
	}
}

$a = 'foo::';
$a();

?>
--EXPECTF--
Fatal error: Call to undefined function foo::() in %s on line %d
