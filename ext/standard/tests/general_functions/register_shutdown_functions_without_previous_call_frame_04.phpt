--TEST--
register_shutdown_function() without a previous call frame 04
--FILE--
<?php
register_shutdown_function("func_get_arg");
?>
Done
--EXPECT--
Done

Fatal error: Uncaught ArgumentCountError: func_get_arg() expects exactly 1 argument, 0 given in [no active file]:0
Stack trace:
#0 [internal function]: func_get_arg()
#1 {main}
  thrown in [no active file] on line 0
