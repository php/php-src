--TEST--
Parameter type hint - numeric type hints
--FILE--
<?php

function x($error, $msg) { var_dump($msg); }
set_error_handler('x', E_RECOVERABLE_ERROR);

function foo(numeric $param) { var_dump($param); }

foo(1);
foo(true);
foo(NULL);
foo(array(1));
foo("foo");
foo("123.33");
foo("123");
foo("");
foo(111.222);
foo(new Stdclass);
foo(tmpfile());

?>
--EXPECTF--
int(1)
string(%d) "Argument 1 passed to foo() must be of the type numeric, boolean given, called in %s on line %d and defined"
bool(true)
string(%d) "Argument 1 passed to foo() must be of the type numeric, null given, called in %s on line %d and defined"
NULL
string(%d) "Argument 1 passed to foo() must be of the type numeric, array given, called in %s on line %d and defined"
array(1) {
  [0]=>
  int(1)
}
string(%d) "Argument 1 passed to foo() must be of the type numeric, string given, called in %s on line %d and defined"
string(3) "foo"
string(6) "123.33"
string(3) "123"
string(%d) "Argument 1 passed to foo() must be of the type numeric, string given, called in %s on line %d and defined"
string(0) ""
float(111.222)
string(%d) "Argument 1 passed to foo() must be of the type numeric, object given, called in %s on line %d and defined"
object(stdClass)#1 (0) {
}
string(%d) "Argument 1 passed to foo() must be of the type numeric, resource given, called in %s on line %d and defined"
resource(4) of type (stream)
