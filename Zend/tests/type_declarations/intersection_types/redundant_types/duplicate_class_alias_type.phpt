--TEST--
Duplicate class alias type
--FILE--
<?php

use A as B;
function foo(): A&B {}

?>
--EXPECTF--
Fatal error: Duplicate type A is redundant in %s on line %d
