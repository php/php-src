--TEST--
A less restrictive type constrain is part of the DNF type 002
--FILE--
<?php

interface A {}
interface B {}

function test(): A|(A&B) {}

?>
===DONE===
--EXPECTF--
Fatal error: Type A is less restrictive than type A&B in %s on line %d
