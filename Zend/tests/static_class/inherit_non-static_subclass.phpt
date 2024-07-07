--TEST--
Tests that a non-static class cannot inherit from a static class
--FILE--
<?php

static class C {}

class C2 extends C {}
?>
--EXPECTF--
Fatal error: Non-static class C2 cannot extend static class C in %s on line %d
