--TEST--
'boolean' cannot be used as a class, interface or trait name - class_alias
--FILE--
<?php

class foobar {}
class_alias('foobar', 'boolean');
--EXPECTF--
Fatal error: Cannot use 'boolean' as class name as it is reserved in %s on line %d