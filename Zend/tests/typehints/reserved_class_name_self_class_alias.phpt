--TEST--
'self' cannot be used as a class, interface or trait name - class_alias
--FILE--
<?php

class foobar {}
class_alias('foobar', 'self');
--EXPECTF--
Fatal error: Cannot use 'self' as class name as it is reserved in %s on line %d