--TEST--
'integer' cannot be used as a class, interface or trait name - trait
--FILE--
<?php

trait integer {}
--EXPECTF--
Fatal error: Cannot use 'integer' as class name as it is reserved in %s on line %d