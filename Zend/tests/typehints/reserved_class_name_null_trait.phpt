--TEST--
'null' cannot be used as a class, interface or trait name - trait
--FILE--
<?php

trait null {}
--EXPECTF--
Fatal error: Cannot use 'null' as class name as it is reserved in %s on line %d