--TEST--
Error trying to add attributes to multiple constants at once
--FILE--
<?php

#[\Foo]
const First = 1,
    Second = 2;

?>
--EXPECTF--
Fatal error: Cannot apply attributes to multiple constants at once in %s on line %d
