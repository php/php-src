--TEST--
'true' cannot be used as a class, interface or trait name - class
--FILE--
<?php

class true {}
--EXPECTF--
Fatal error: Cannot use 'true' as class name as it is reserved in %s on line %d