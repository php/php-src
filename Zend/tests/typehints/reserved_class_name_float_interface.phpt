--TEST--
'float' cannot be used as a class, interface or trait name - interface
--FILE--
<?php

interface float {}
--EXPECTF--
Fatal error: Cannot use 'float' as class name as it is reserved in %s on line %d