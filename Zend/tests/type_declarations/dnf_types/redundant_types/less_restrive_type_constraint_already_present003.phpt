--TEST--
A less restrictive type constrain is part of the DNF type 003
--FILE--
<?php

interface A {}
interface B {}
interface C {}

function test(): (A&B)|(A&B&C) {}

?>
===DONE===
--EXPECTF--
Fatal error: Type A&B&C is redundant as it is more restrictive than type A&B in %s on line %d
