--TEST--
Bug #61362 (Exception::getTraceAsString, Exception::__toString not able to handle unicode)
--FILE--
<?php
function test($arg){
	throw new Exception();
}

try {
	test('тест');
}
catch(Exception $e) {
	echo $e->getTraceAsString(), "\n";
	echo (string)$e;
}
?>
--EXPECTF--
#0 %s(%d): test('\xD1\x82\xD0\xB5\xD1\x81\xD1\x82')
#1 {main}
Exception in %s:%d
Stack trace:
#0 %s(%d): test('\xD1\x82\xD0\xB5\xD1\x81\xD1\x82')
#1 {main}
