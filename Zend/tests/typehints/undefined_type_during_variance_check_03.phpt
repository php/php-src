--TEST--
Undefined types during variance checks are errors
--FILE--
<?php

abstract class X {
  abstract function m(stdClass $z);
}

class Y extends X {
  function m(UndefinedA $z) {}
}
?>
--EXPECTF--
Fatal error: Declaration of Y::m(UndefinedA $z) must be compatible with X::m(stdClass $z) in %s on line %d
