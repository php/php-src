--TEST--
Tests passing default beyond the maximum parameters specified by the function
--FILE--
<?php

function F() {}
F(default);
?>
--EXPECTF--
Fatal error: Uncaught ValueError: Cannot pass default to undeclared parameter 1 of F() in %s:%d
Stack trace:
%a
