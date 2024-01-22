--TEST--
register_shutdown_function() without a previous call frame 01
--FILE--
<?php
register_shutdown_function("forward_static_call", "hash_hkdf");
?>
Done
--EXPECT--
Done

Fatal error: Uncaught Error: Cannot call forward_static_call() when no class scope is active in [no active file]:0
Stack trace:
#0 [internal function]: forward_static_call('hash_hkdf')
#1 {main}
  thrown in [no active file] on line 0
