--TEST--
Using "_" as a class name alias is deprecated
--FILE--
<?php

class_alias('stdClass', '_');

class_alias('stdClass', 'Foo\\_');

?>
--EXPECTF--
Deprecated: Using "_" as a class alias is deprecated since 8.4 in %s on line %d

Deprecated: Using "_" as a class alias is deprecated since 8.4 in %s on line %d
