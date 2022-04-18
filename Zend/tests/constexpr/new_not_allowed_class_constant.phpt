--TEST--
New not allowed in class constant
--FILE--
<?php

// New in class constants (and static properties) brings up evaluation order questions: When
// should the (potentially side-effecting) new expression be evaluated? Evaluating it when the
// class is declared would break references to classes that are declared later in the same
// file. On the other hand, the current lazy evaluation of initializers is somewhat ill-defined
// when we start considering side-effecting expressions.

class Test {
    const X = new stdClass;
}

?>
--EXPECTF--
Fatal error: New expressions are not supported in this context in %s on line %d
