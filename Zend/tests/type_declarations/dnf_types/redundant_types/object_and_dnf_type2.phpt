--TEST--
A DNF type which contains object is redundant 2
--FILE--
<?php

interface A {}
interface B {}

function test(): object|(A&B) {}

?>
===DONE===
--EXPECTF--
Fatal error: Type (A&B)|object contains both object and a class type, which is redundant in %s on line %d
