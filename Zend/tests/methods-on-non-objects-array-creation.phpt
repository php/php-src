--TEST--
Catch method calls on non-objects inside array creation
--FILE--
<?php
set_error_handler(function($code, $message) {
  var_dump($code, $message);
});

$x= null;
var_dump([$x->method() => 'OK']);
var_dump([$x->method(), $x->method(), $x->method()]);
echo "Alive\n";
?>
--EXPECTF--
int(4096)
string(%d) "Call to a member function method() on null"
array(1) {
  [""]=>
  string(2) "OK"
}
int(4096)
string(%d) "Call to a member function method() on null"
int(4096)
string(%d) "Call to a member function method() on null"
int(4096)
string(%d) "Call to a member function method() on null"
array(3) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  NULL
}
Alive