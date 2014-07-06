--TEST--
Catch method calls on non-objects with nested calls to new
--FILE--
<?php
class Nesting {
}
set_error_handler(function($code, $message) {
  static $i= 0;
  echo 'Called #'.(++$i)."\n";
});

$x= null;
var_dump($x->method(new Nesting()));
var_dump($x->method(new Nesting(), new Nesting()));
var_dump($x->method(new Nesting(new Nesting())));
var_dump($x->method(new Nesting($x->nested())));
var_dump($x->method(new Nesting($x->nested(new Nesting()))));
var_dump($x->method($x->nested(new Nesting($x->deep()))));
var_dump($x->method([new Nesting()]));
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
Alive
