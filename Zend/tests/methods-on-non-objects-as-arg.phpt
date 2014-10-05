--TEST--
Catch method calls on non-objects as argument
--FILE--
<?php
function nesting() {
  return func_get_args();
}
set_error_handler(function($code, $message) {
  static $i= 0;
  echo 'Called #'.(++$i)."\n";
});

$x= null;
var_dump(nesting($x->method()));
var_dump(nesting(nesting($x->method())));
var_dump(nesting($x->method(nesting($x->method()))));
var_dump(nesting($x->method(), $x->method()));
echo "Alive\n";
?>
--EXPECTF--
Called #1
array(1) {
  [0]=>
  NULL
}
Called #2
array(1) {
  [0]=>
  array(1) {
    [0]=>
    NULL
  }
}
Called #3
array(1) {
  [0]=>
  NULL
}
Called #4
Called #5
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}
Alive
