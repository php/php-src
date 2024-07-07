--TEST--
Tests that a static class cannot be instantiated via reflection
--FILE--
<?php

static class C {}

new ReflectionClass('C')->newInstance();
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot instantiate static class C in %s:%d
Stack trace:%a
