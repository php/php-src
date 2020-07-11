--TEST--
Explicit nullable types do not imply a default value
--FILE--
<?php

function f(?callable $p) {}

f();
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: Too few arguments to function f(), 0 passed in %snullable_type_parameters_do_not_have_default_value.php on line %d and exactly 1 expected in %s:%d
Stack trace:
#%d %s
#%d %s
  thrown in %s on line %d
