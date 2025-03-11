--TEST--
Bug #77738 (Nullptr deref in zend_compile_expr)
--FILE--
<?php
__COMPILER_HALT_OFFSET__;
; // <- important
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "__COMPILER_HALT_OFFSET__" in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
