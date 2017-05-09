--TEST--
'long' cannot be used as a class, interface or trait name - trait
--FILE--
<?php

trait long {}
--EXPECTF--
Fatal error: Cannot use 'long' as class name as it is reserved in %s on line %d