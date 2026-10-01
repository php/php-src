--TEST--
Using "let" as a class name alias is deprecated
--FILE--
<?php

class_alias('stdClass', 'let');

class_alias('stdClass', 'Foo\\let');

?>
--EXPECTF--
Deprecated: Using "let" as a class alias is deprecated since 8.6 in %s on line %d

Deprecated: Using "let" as a class alias is deprecated since 8.6 in %s on line %d
