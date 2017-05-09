--TEST--
'long' cannot be used as a class, interface or trait name - interface
--FILE--
<?php

interface long {}
--EXPECTF--
Fatal error: Cannot use 'long' as class name as it is reserved in %s on line %d