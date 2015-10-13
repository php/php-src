--TEST--
'true' cannot be used as a class, interface or trait name - trait
--FILE--
<?php

trait true {}
--EXPECTF--
Fatal error: Cannot use 'true' as class name as it is reserved in %s on line %d