--TEST--
non-consecutive type decls are still checked for variance
--FILE--
<?php

interface A {
  function m(): A;
}

// "echo 'hi';" is not a declaration statement
echo 'hi';

class B implements A {
  function m(): stdClass { return $this; }
}

echo get_class((new B())->m());
?>
--EXPECTF--
hi
Fatal error: Declaration of B::m(): stdClass must be compatible with A::m(): A in %s on line %d
