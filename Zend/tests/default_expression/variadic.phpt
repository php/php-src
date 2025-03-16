--TEST--
Tests passing default to a variadic parameter
--FILE--
<?php

(fn (...$V) => $V)(default);
?>
--EXPECTF--
Fatal error: Uncaught ValueError: Cannot pass default to undeclared parameter 1 of {closure:%s:%d}() in %s:%d
Stack trace:
%a
