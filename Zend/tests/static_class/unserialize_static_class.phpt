--TEST--
Tests that a static class cannot be created via unserialize()
--FILE--
<?php

static class C {}

unserialize('O:1:"C":0:{}');
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot instantiate static class C in %s:%d
Stack trace:%a
