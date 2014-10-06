--TEST--
Catch method calls on non-objects with nested non-compile-time-resolveable calls
--FILE--
<?php
require('methods-on-non-objects-nested.inc');

set_error_handler(function($code, $message) {
  static $i= 0;
  echo 'Called #'.(++$i)."\n";
});

$x= null;

var_dump($x->method(nested()));

$closure= function() { return nested(); };
var_dump($x->method($closure()));

$lambda= create_function('', 'return nested();');
var_dump($x->method($lambda()));

$func= 'nested';
var_dump($x->method($func()));

var_dump($x->method(call_user_func('nested')));
var_dump($x->method(call_user_func_array('nested', [])));

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
Called #5
NULL
Called #6
NULL
Alive
