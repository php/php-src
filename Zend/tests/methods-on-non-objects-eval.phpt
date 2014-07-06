--TEST--
Indirect call inside eval to member function on non-object
--FILE--
<?php
set_error_handler(function($code, $message) {
  var_dump($code, $message);
});

$x= null;
var_dump(eval('$x->method(1, 2, 3);'));
echo "Alive\n";
?>
--EXPECTF--

int(4096)
string(%d) "Call to a member function method() on null"
NULL
Alive
