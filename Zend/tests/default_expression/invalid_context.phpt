--TEST--
Tests specifying the default keyword in an invalid context throws a compile error
--FILE--
<?php

default;
?>
--EXPECTF--
Fatal error: Cannot use default in non-argument context. in %s on line %d
