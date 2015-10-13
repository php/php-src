--TEST--
'boolean' cannot be used as a class, interface or trait name - class
--FILE--
<?php

class boolean {}
--EXPECTF--
Fatal error: Cannot use 'boolean' as class name as it is reserved in %s on line %d