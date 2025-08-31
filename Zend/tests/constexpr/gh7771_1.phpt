--TEST--
GH-7771 (Incorrect file/line for class constant expression exceptions)
--FILE--
<?php

include __DIR__ . '/gh7771_1_definition.inc';

new Foo();

?>
--EXPECTF--
Fatal error: Uncaught Error: Class "NonExistent" not found in %sgh7771_1_definition.inc:4
Stack trace:
#0 %sgh7771_1.php(5): [constant expression]()
#1 {main}
  thrown in %sgh7771_1_definition.inc on line 4
