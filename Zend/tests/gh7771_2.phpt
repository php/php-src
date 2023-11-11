--TEST--
GH-7771 (Incorrect file/line for class constant expression exceptions)
--FILE--
<?php

include __DIR__ . '/gh7771_2_definition.inc';

new Foo();

?>
--EXPECTF--
Fatal error: Uncaught Error: Class "NonExistent" not found in %sgh7771_2_definition.inc:6
Stack trace:
#0 {main}
  thrown in %sgh7771_2_definition.inc on line 6
