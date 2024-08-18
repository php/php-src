--TEST--
Tests passing default to a required parameter of an internal class
--FILE--
<?php

DateTime::createFromInterface(default);
?>
--EXPECTF--
Fatal error: Uncaught ValueError: Cannot pass default to required parameter 1 of DateTime::createFromInterface() in %s:%d
Stack trace:%a
