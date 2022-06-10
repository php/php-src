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
Fatal error: Type A&B is redundant as it is more restrictive than type A in %s on line %d
