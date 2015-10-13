--TEST--
'bool' cannot be used as a class, interface or trait name - trait
--FILE--
<?php

trait bool {}
--EXPECTF--
Fatal error: Cannot use 'bool' as class name as it is reserved in %s on line %d