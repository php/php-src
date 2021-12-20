--TEST--
A less restrictive type constrain is part of the DNF type 001
--FILE--
<?php

interface A {}
interface B {}

function test(): (A&B)|A {}

?>
===DONE===
--EXPECTF--
Fatal error: Type A is less restrictive than type A&B in %s on line %d
