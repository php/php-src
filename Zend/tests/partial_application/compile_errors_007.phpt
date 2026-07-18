--TEST--
PFA compile errors: positional after a named placeholder frees pending named positions
--DESCRIPTION--
A named placeholder allocates the named-positions table; a following
positional placeholder is a compile error. The table must not leak on
that bailout. The leak is only visible under valgrind/ASAN; this test
exercises the path so CI catches a regression.
--FILE--
<?php
function foo($a, $b) {}
$p = foo(b: ?, ?);
?>
--EXPECTF--
Fatal error: Cannot use positional argument after named argument in %s on line %d
