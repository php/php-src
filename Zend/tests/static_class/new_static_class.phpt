--TEST--
Tests that a static class cannot be instantiated
--FILE--
<?php

static class C {}

new C;
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot instantiate static class C in %s:%d
Stack trace:%a
