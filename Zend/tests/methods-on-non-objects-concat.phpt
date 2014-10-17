--TEST--
Catch method calls on non-objects inside concatenation
--FILE--
<?php
set_error_handler(function($code, $message) {
  var_dump($code, $message);
});

$x= null;
echo "Before\n".$x->method()."After\n";
echo "Alive\n";
?>
--EXPECTF--
int(4096)
string(%d) "Call to a member function method() on null"
Before
After
Alive