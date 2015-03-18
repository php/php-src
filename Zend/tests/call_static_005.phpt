--TEST--
Invalid method name in dynamic static call
--FILE--
<?php

class foo {
	static function __callstatic($a, $b) {
		var_dump($a);
	}
}

try {
	$a = 'foo::';
	$a();
} catch (EngineException $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
Call to undefined function foo::()
