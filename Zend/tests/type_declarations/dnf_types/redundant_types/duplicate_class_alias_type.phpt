--TEST--
Duplicate class alias type
--FILE--
<?php

interface X {}

use A as B;
function foo(): (X&A)|(X&B) {}

?>
--EXPECTF--
Fatal error: Type X&A is redundant with type X&A in %s on line %d
