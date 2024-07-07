--TEST--
Tests that a static class can inherit from another static class
--FILE--
<?php

static class C {}

static class C2 extends C {}
?>
--EXPECT--
