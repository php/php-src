--TEST--
Bug #71221 (Null pointer deref (segfault) in get_defined_vars via ob_start)
--FILE--
<?php
register_shutdown_function("get_defined_vars");
?>
--EXPECT--
Fatal error: Uncaught Error: Cannot call get_defined_vars() dynamically in [no active file]:0
Stack trace:
#0 [internal function]: get_defined_vars()
#1 {main}
  thrown in [no active file] on line 0
