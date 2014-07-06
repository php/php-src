--TEST--
Catch method calls on non-objects with nested function and method calls
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
$ref= 'nested';
$func= function() { return nested(); };
var_dump($x->method(nested()));
var_dump($x->method(nested(), nested()));
var_dump($x->method(nested(nested())));
var_dump($x->method($x->nested()));
var_dump($x->method($x->nested(), $x->nested()));
var_dump($x->method($x->nested(nested())));
var_dump($x->method($x->nested($x->deep())));
var_dump($x->method($x->nested(nested($x->deep()))));
var_dump($x->method(nested(nested($x->nested()))));
var_dump($x->method($ref()));
var_dump($x->method($func()));
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
Called #7
NULL
Called #8
NULL
Called #9
NULL
Called #10
NULL
Called #11
NULL
Called #12
NULL
Alive
