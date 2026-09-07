--TEST--
PFA compile errors: placeholders can not appear after named args
--FILE--
<?php
foo(n: 5, ?);
?>
--EXPECTF--
Fatal error: Cannot use positional argument after named argument in %s on line %d
