--TEST--
'parent' cannot be used as a class, interface or trait name - interface
--FILE--
<?php

interface parent {}
--EXPECTF--
Fatal error: Cannot use 'parent' as class name as it is reserved in %s on line %d