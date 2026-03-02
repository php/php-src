--TEST--
A DNF type which contains object is redundant
--FILE--
<?php

interface A {}
interface B {}

function test(): (A&B)|object {}

?>
===DONE===
--EXPECTF--
Fatal error: Type (A&B)|object contains both object and a class type, which is redundant in %s on line %d
