--TEST--
Unterminated comment
--FILE--
<?php
/* Foo
Bar
--EXPECTF--
Parse error: Unterminated comment starting line 2 in %s on line %d
