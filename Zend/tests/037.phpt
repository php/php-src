--TEST--
Trying to access inexistent static property of Closure
--FILE--
<?php

namespace closure;

class closure { static $x = 1;}

$x = __NAMESPACE__;
var_dump(closure::$x);

var_dump($x::$x);

?>
--EXPECTF--
int(1)

Fatal error: Uncaught Error: Access to undeclared static property: Closure::$x in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
