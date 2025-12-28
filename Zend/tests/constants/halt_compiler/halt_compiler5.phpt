--TEST--
Using __COMPILER_HALF_OFFSET__ with trailing {} (OSS-Fuzz #17895)
--FILE--
<?php
__COMPILER_HALT_OFFSET__;
{}
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "__COMPILER_HALT_OFFSET__" in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
