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
Fatal error: Type A&B is redundant as it is more restrictive than type A in %s on line %d
