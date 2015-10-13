--TEST--
'double' cannot be used as a class, interface or trait name - trait
--FILE--
<?php

trait double {}
--EXPECTF--
Fatal error: Cannot use 'double' as class name as it is reserved in %s on line %d