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

Fatal error: Uncaught Error: Class "closure" not found in %s:%d
Stack trace:
#%d {main}
  thrown in %s on line %d
