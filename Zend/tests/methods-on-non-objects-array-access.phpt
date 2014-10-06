--TEST--
Catch method calls on non-objects inside array access
--FILE--
<?php
set_error_handler(function($code, $message) {
  var_dump($code, $message);
});

$x= null;
$a= [null => 'OK'];
var_dump($a[$x->method()]);
echo "Alive\n";
?>
--EXPECTF--
int(4096)
string(%d) "Call to a member function method() on null"
string(2) "OK"
Alive