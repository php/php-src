--TEST--
Using "is" as a class name alias is deprecated
--FILE--
<?php

class_alias('stdClass', 'is');

class_alias('stdClass', 'Foo\\is');

?>
--EXPECTF--
Deprecated: Using "is" as a class alias is deprecated since 8.6 in %s on line %d

Deprecated: Using "is" as a class alias is deprecated since 8.6 in %s on line %d
