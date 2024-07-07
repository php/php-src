--TEST--
Tests that a static class cannot inherit from a non-static class
--FILE--
<?php

class C {}

static class C2 extends C {}
?>
--EXPECTF--
Fatal error: Static class C2 cannot extend non-static class C in %s on line %d
