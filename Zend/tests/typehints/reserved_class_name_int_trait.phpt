--TEST--
'int' cannot be used as a class, interface or trait name - trait
--FILE--
<?php

trait int {}
--EXPECTF--
Fatal error: Cannot use 'int' as class name as it is reserved in %s on line %d