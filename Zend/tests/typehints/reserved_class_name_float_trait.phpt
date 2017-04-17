--TEST--
'float' cannot be used as a class, interface or trait name - trait
--FILE--
<?php

trait float {}
--EXPECTF--
Fatal error: Cannot use 'float' as class name as it is reserved in %s on line %d