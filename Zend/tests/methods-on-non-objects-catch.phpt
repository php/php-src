--TEST--
Catch method calls on non-objects raise recoverable errors
--FILE--
<?php
set_error_handler(function($code, $message) {
  var_dump($code, $message);
});

$x= null;
try {
	var_dump($x->method());
} catch (Error $e) {
  var_dump($e->getCode(), $e->getMessage());
}
echo "Alive\n";
?>
--EXPECTF--
int(0)
string(%d) "Call to a member function method() on null"
Alive
