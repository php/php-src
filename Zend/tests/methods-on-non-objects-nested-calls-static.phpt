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
var_dump($x->method(Nesting::nested()));
echo "Alive\n";
?>
--EXPECTF--
Called #1
NULL
Alive
