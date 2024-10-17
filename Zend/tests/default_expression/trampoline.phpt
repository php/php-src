--TEST--
Tests passing default to a trampoline function
--FILE--
<?php

(fn ($P = 1) => $P)->__invoke(default);
?>
--EXPECTF--
Fatal error: Uncaught Exception: Unable to fetch default value in %s:%d
Stack trace:
%a
