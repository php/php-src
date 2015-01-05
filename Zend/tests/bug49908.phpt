--TEST--
Bug #49908 (throwing exception in __autoload crashes when interface is not defined)
--FILE--
<?php

function __autoload($className) {
	var_dump($className);
	
	if ($className == 'Foo') {
		class Foo implements Bar {};
	} else {
		throw new Exception($className);
	}
}

new Foo;

?>
--EXPECTF--
%unicode|string%(3) "Foo"
%unicode|string%(3) "Bar"

Fatal error: Uncaught exception 'Exception' with message 'Bar' in %s:%d
Stack trace:
#0 %s(7): __autoload('Bar')
#1 %s(13): __autoload('Foo')
#2 {main}
  thrown in %s on line %d
