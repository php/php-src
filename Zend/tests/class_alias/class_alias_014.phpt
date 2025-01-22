--TEST--
Testing creation of alias to class name without namespace prefix
--FILE--
<?php

namespace foo;

class bar {
}

class_alias('bar', 'baz');

?>
--EXPECTF--
Warning: Class "bar" not found in %s on line %d
