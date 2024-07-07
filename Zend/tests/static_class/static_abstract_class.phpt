--TEST--
Tests that a static class cannot be marked abstract
--FILE--
<?php

abstract static class C {}
?>
--EXPECTF--
Fatal error: Cannot use the static modifier on an abstract class in %s on line %d
