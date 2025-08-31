--TEST--
Function declaration should not be allowed before namespace declaration
--FILE--
<?php

function foo() {}

namespace Bar;

?>
--EXPECTF--
Fatal error: Namespace declaration statement has to be the very first statement or after any declare call in the script in %s on line %d
