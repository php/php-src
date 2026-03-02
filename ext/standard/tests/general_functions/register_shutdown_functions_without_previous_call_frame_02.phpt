--TEST--
register_shutdown_function() without a previous call frame 02
--FILE--
<?php
register_shutdown_function("func_get_args");
?>
Done
--EXPECT--
Done

Fatal error: Uncaught Error: Cannot call func_get_args() dynamically in [no active file]:0
Stack trace:
#0 [internal function]: func_get_args()
#1 {main}
  thrown in [no active file] on line 0
