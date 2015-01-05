--TEST--
Testing creation of alias to global scope
--FILE--
<?php

namespace foo;

class bar {
}

class_alias('foo\bar', 'foo');

var_dump(new \foo);
var_dump(new foo);

?>
--EXPECTF--
object(foo\bar)#%d (0) {
}

Fatal error: Class 'foo\foo' not found in %s on line %d
