--TEST--
Intersection types cannot be implicitly nullable
--FILE--
<?php

function foo(X&Y $foo = null) {}

?>
--EXPECTF--
Fatal error: Cannot use null as default value for parameter $foo of type X&Y in %s on line %d
