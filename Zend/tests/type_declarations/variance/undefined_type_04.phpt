--TEST--
Undefined types during variance checks are errors
--FILE--
<?php

interface X {
  function m(stdClass $z);
}

class Y implements X {
  function m(UndefinedA $z) {}
}
?>
--EXPECTF--
Fatal error: Declaration of Y::m(UndefinedA $z) must be compatible with X::m(stdClass $z) in %s on line %d
