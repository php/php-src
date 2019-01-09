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
  function m(): B { return $this; }
}

echo get_class((new B())->m());
?>
--EXPECT--
hiB
