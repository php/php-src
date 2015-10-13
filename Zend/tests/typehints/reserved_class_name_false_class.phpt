--TEST--
'false' cannot be used as a class, interface or trait name - class
--FILE--
<?php

class false {}
--EXPECTF--
Fatal error: Cannot use 'false' as class name as it is reserved in %s on line %d