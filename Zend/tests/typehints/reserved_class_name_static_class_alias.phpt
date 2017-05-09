--TEST--
'static' cannot be used as a class, interface or trait name - class_alias
--FILE--
<?php

class foobar {}
class_alias('foobar', 'static');
--EXPECTF--
Fatal error: Cannot use 'static' as class name as it is reserved in %s on line %d