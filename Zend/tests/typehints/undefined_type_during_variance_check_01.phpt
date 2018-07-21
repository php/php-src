--TEST--
Undefined types during variance checks are warnings
--FILE--
<?php

class X {
  function m(stdClass $z) {}
}

class Y extends X {
  function m(UndefinedA $z) {}
}
?>
--EXPECTF--
Warning: Declaration of Y::m(UndefinedA $z) should be compatible with X::m(stdClass $z) in %s on line %d
