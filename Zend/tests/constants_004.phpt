--TEST--
Trying to redeclare constant inside namespace
--FILE--
<?php

namespace foo;

const foo = 1;
const foo = 2;

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Constant foo\foo already defined in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
