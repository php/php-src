--TEST--
Defining a free-standing assert() function is deprecated
--FILE--
<?php

namespace FooBar;

function assert() {}

?>
--EXPECTF--
Fatal error: Defining a custom assert() function is not allowed, as the function has special semantics in %s on line %d
