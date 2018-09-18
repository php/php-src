--TEST--
Defining a free-standing assert() function is deprecated
--FILE--
<?php

namespace FooBar;

function assert() {}

?>
--EXPECTF--
Deprecated: Defining a custom assert() function is deprecated, as the function has special semantics in %s on line %d
