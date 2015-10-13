--TEST--
'self' cannot be used as a class, interface or trait name - interface
--FILE--
<?php

interface self {}
--EXPECTF--
Fatal error: Cannot use 'self' as class name as it is reserved in %s on line %d