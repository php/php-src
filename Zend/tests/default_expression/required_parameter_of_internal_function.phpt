--TEST--
Tests passing default to a required parameter of an internal function
--FILE--
<?php

json_encode(default);
?>
--EXPECTF--
Fatal error: Uncaught ValueError: Cannot pass default to required parameter 1 of json_encode() in %s:%d
Stack trace:
%a
