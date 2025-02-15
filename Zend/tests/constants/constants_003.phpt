--TEST--
Using namespace constants and constants of global scope
--FILE--
<?php

namespace foo;

const foo = 1;

define('foo', 2);

var_dump(foo, namespace\foo, \foo\foo, \foo, constant('foo'), constant('foo\foo'));

?>
--EXPECT--
int(1)
int(1)
int(1)
int(2)
int(2)
int(1)
