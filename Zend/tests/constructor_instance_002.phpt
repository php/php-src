--TEST--
Not allowed to call the constructor on an object instance
--FILE--
<?php
class C {
  function __construct() {}
}
$c = new C();
$constr = '__construct';
$c->$constr();
--EXPECTF--
Fatal error: Uncaught Error: Cannot call the constructor on an object instance %s
Stack trace:
#0 {main}
  thrown %s
