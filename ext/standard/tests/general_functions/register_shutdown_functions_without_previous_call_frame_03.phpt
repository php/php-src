--TEST--
register_shutdown_function() without a previous call frame 03
--FILE--
<?php
register_shutdown_function("func_num_args");
?>
Done
--EXPECT--
Done

Fatal error: Uncaught Error: Cannot call func_num_args() dynamically in [no active file]:0
Stack trace:
#0 [internal function]: func_num_args()
#1 {main}
  thrown in [no active file] on line 0
