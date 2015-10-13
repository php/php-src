--TEST--
'parent' cannot be used as a class, interface or trait name - class
--FILE--
<?php

class parent {}
--EXPECTF--
Fatal error: Cannot use 'parent' as class name as it is reserved in %s on line %d