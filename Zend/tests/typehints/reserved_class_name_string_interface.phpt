--TEST--
'string' cannot be used as a class, interface or trait name - interface
--FILE--
<?php

interface string {}
--EXPECTF--
Fatal error: Cannot use 'string' as class name as it is reserved in %s on line %d