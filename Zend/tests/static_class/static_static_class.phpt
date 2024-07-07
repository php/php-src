--TEST--
Tests that a static class cannot be so marked more than once
--FILE--
<?php

static static class C {}
?>
--EXPECTF--
Fatal error: Multiple static modifiers are not allowed in %s on line %d
