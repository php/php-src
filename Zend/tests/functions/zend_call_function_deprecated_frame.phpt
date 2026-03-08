--TEST--
Deprecated function notice promoted to exception within zend_call_function()
--FILE--
<?php

#[Deprecated]
function foo(string $v) {
	return $v . '!';
}

set_error_handler(function ($number, $message) {
	throw new Exception($message);
});

$a = array_map(foo(...), ['Hello', 'World']);
var_dump($a);

?>
--EXPECTF--
Fatal error: Uncaught Exception: Function foo() is deprecated in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(16384, 'Function foo() ...', '%s', %d)
#1 {main}
  thrown in %s on line %d
