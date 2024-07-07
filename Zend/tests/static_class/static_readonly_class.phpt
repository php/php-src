--TEST--
Tests that a static class cannot be marked readonly
--FILE--
<?php

static readonly class C {}
?>
--EXPECTF--
Fatal error: Cannot use the static modifier on a readonly class in %s on line %d
