--TEST--
helpful error messages when using non-existant typehint aliases
--FILE--
<?php
require 'common.inc';

function foo(boolean $foo) {}
function bar(integer $bar) {}
function qux(double $qux) {}

foo(true);
bar(1);
qux(1.0);
?>
--EXPECTF--
E_RECOVERABLE_ERROR: Argument 1 passed to foo() must be an instance of boolean, boolean (did you mean to use the `bool` typehint?) given, called in %s on line 8 and defined in %s on line 4
E_RECOVERABLE_ERROR: Argument 1 passed to bar() must be an instance of integer, integer (did you mean to use the `int` typehint?) given, called in %s on line 9 and defined in %s on line 5
E_RECOVERABLE_ERROR: Argument 1 passed to qux() must be an instance of double, float (did you mean to use the `float` typehint?) given, called in %s on line 10 and defined in %s on line 6