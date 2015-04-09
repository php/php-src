--TEST--
Catch method calls on non-objects with nested calls to namespaced functions with core counterparts
--FILE--
<?php namespace test;
function strlen($str) {
  throw new LogicException('Should not be called');
}
set_error_handler(function($code, $message) {
  static $i= 0;
  echo 'Called #'.(++$i)."\n";
});

$x= null;
var_dump($x->method(strlen('Test')));
var_dump($x->method(strlen('Test'), strlen('Test')));
var_dump($x->method([strlen('Test')]));
echo "Alive\n";
?>
--EXPECTF--
Called #1
NULL
Called #2
NULL
Called #3
NULL
Alive
