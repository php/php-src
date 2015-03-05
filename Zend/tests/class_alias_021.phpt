--TEST--
Overriding internal class with class alias
--FILE--
<?php

namespace foo;

use \baz as stdclass;

class bar { }

class_alias('foo\bar', 'baz');

var_dump(new bar);
var_dump(new stdClass);
var_dump(new \baz);

?>
--EXPECTF--
object(foo\bar)#%d (0) {
}
object(foo\bar)#%d (0) {
}
object(foo\bar)#%d (0) {
}
