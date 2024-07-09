--TEST--
Tests that an anonymous class cannot be marked static
--FILE--
<?php

new static class C {}
?>
--EXPECTF--
Fatal error: Cannot use the static modifier on an anonymous class in %s on line %d
