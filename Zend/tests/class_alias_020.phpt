--TEST--
Testing class alias in multiple namespaces
--FILE--
<?php

namespace foo;


class foo {
}

class_alias(__NAMESPACE__ .'\foo', 'foo');

namespace foo\bar;

class foo { 
}

class_alias(__NAMESPACE__ .'\foo', 'bar');


var_dump(new \foo, new \bar);

var_dump(new \foo\foo, new \foo\bar);

?>
--EXPECTF--
object(foo\foo)#1 (0) {
}
object(foo\bar\foo)#2 (0) {
}

Fatal error: Uncaught Error: Class 'foo\bar' not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
