--TEST--
'parent' cannot be used as a class, interface or trait name - trait
--FILE--
<?php

trait parent {}
--EXPECTF--
Fatal error: Cannot use 'parent' as class name as it is reserved in %s on line %d