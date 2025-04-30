--TEST--
Testing alias of alias
--FILE--
<?php

namespace test\baz;

class foo {
}

new foo;

$alias1 = __NAMESPACE__ .'\T';
class_alias(__NAMESPACE__ .'\foo', $alias1);

$alias2 = $alias1 .'\BAR';
class_alias($alias1, $alias2);

var_dump(new \test\baz\foo, new \test\baz\T\BAR);

?>
--EXPECTF--
object(test\baz\foo)#%d (0) {
}
object(test\baz\foo)#%d (0) {
}
