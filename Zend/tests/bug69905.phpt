--TEST--
Bug #69905 (null ptr deref and segfault in ZEND_FETCH_DIM_RW_SPEC_VAR_UNUSED_HANDLER)
--FILE--
<?php
md5(0)[]--;
?>
--EXPECTF--
Fatal error: Uncaught Error: [] operator not supported for strings in %sbug69905.php:2
Stack trace:
#0 {main}
  thrown in %sbug69905.php on line 2
