--TEST--
Tests that a static class can be marked final
--FILE--
<?php

final static class C {}

static class C2 extends C {}
?>
--EXPECTF--
Fatal error: Class C2 cannot extend final class C in %s on line %d
