--TEST--
Catch chained method calls on non-objects raise recoverable errors
--FILE--
<?php
set_error_handler(function($code, $message) {
  var_dump($code, $message);
});

$x= null;
var_dump($x->method()->chained()->invocations());
echo "Alive\n";
?>
--EXPECTF--

int(4096)
string(%d) "Call to a member function method() on null"
int(4096)
string(%d) "Call to a member function chained() on null"
int(4096)
string(%d) "Call to a member function invocations() on null"
NULL
Alive
