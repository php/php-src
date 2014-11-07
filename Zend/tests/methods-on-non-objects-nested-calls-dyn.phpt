--TEST--
Catch method calls on non-objects with nested dynamic calls
--FILE--
<?php
function nested() {
  throw new LogicException('Should not be called');
}
set_error_handler(function($code, $message) {
  static $i= 0;
  echo 'Called #'.(++$i)."\n";
});

$x= null;

$closure= function() { return nested(); };
var_dump($x->method($closure()));

$lambda= create_function('', 'return nested();');
var_dump($x->method($lambda()));

$func= 'nested';
var_dump($x->method($func()));

var_dump($x->method(call_user_func('nested')));

echo "Alive\n";
?>
--EXPECTF--
Called #1
NULL
Called #2
NULL
Called #3
NULL
Called #4
NULL
Alive
