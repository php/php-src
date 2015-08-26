--TEST--
Testing instantiation using namespace:: prefix
--FILE--
<?php

namespace foo;

class bar {
}

class_alias('foo\bar', 'foo\baz');

var_dump(new namespace\baz);

?>
--EXPECTF--
object(foo\bar)#%d (0) {
}
