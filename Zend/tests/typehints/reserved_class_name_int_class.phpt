--TEST--
'int' cannot be used as a class, interface or trait name - class
--FILE--
<?php

class int {}
--EXPECTF--
Fatal error: Cannot use 'int' as class name as it is reserved in %s on line %d