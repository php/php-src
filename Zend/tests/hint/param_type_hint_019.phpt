--TEST--
Parameter type hint - scalar type hints
--FILE--
<?php

function x($error, $msg) { var_dump($msg); }
set_error_handler('x', E_RECOVERABLE_ERROR);

function foo(scalar $param) { var_dump($param); }

foo(1);
foo(true);
foo(NULL);
foo(array(1));
foo("foo");
foo(111.222);
foo(new Stdclass);
foo(tmpfile());

?>
--EXPECTF--
int(1)
bool(true)
string(%d) "Argument 1 passed to foo() must be of the type scalar, null given, called in %s on line %d and defined"
NULL
string(%d) "Argument 1 passed to foo() must be of the type scalar, array given, called in %s on line %d and defined"
array(1) {
  [0]=>
  int(1)
}
string(3) "foo"
float(111.222)
string(%d) "Argument 1 passed to foo() must be of the type scalar, object given, called in %s on line %d and defined"
object(stdClass)#1 (0) {
}
string(%d) "Argument 1 passed to foo() must be of the type scalar, resource given, called in %s on line %d and defined"
resource(4) of type (stream)
