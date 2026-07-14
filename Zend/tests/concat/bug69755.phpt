--TEST--
Bug #69755: segfault in ZEND_CONCAT_SPEC_TMPVAR_CONST_HANDLER
--FILE--
<?php
c . 10;
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "c" in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
