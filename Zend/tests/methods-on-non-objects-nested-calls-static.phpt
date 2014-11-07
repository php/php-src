--TEST--
Catch method calls on non-objects with nested calls to static methods
--FILE--
<?php
class Nesting {
  static function nested() {
    throw new LogicException('Should not be called');
  }
}
set_error_handler(function($code, $message) {
  static $i= 0;
  echo 'Called #'.(++$i)."\n";
});

$x= null;
$class= 'Nesting';
$method= 'nested';
var_dump($x->method(Nesting::nested()));
var_dump($x->method($class::nested()));
var_dump($x->method($class::{$method}()));
var_dump($x->method([Nesting::nested()]));
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
