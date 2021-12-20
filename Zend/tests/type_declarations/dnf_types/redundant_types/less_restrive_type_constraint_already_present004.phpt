--TEST--
A less restrictive type constrain is part of the DNF type 004
--FILE--
<?php

interface A {}
interface B {}
interface C {}

function test(): (A&B&C)|(A&B) {}

?>
===DONE===
--EXPECTF--
Fatal error: Type A&B is less restrictive than type A&B&C in %s on line %d
