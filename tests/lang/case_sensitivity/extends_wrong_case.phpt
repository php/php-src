--TEST--
Extending a class with wrong-case parent name fails with wrong case
--FILE--
<?php
require __DIR__ . "/extends_wrong_case_base.inc";

class Child extends BASECLASS {}
echo "done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Class "BASECLASS" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
