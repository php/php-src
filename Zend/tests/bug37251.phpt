--TEST--
Bug #37251 (deadlock when custom error handler is to catch array type hint error) 
--FILE--
<?php
class Foo {	
	function bar(array $foo) {
	}
}

try {
	$foo = new Foo();
	$foo->bar();
} catch (EngineException $e) {
	echo 'OK';
}
--EXPECT--
OK
