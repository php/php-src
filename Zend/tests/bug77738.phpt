--TEST--
Bug #77738 (Nullptr deref in zend_compile_expr)
--FILE--
<?php
__COMPILER_HALT_OFFSET__;
; // <- important
--EXPECTF--
Warning: Use of undefined constant __COMPILER_HALT_OFFSET__ - assumed '__COMPILER_HALT_OFFSET__' %sbug77738.php on line %d
