--TEST--
strict_types=1 code calling strict_types=0 code
--FILE--
<?php

declare(strict_types=1);

// file that's implicitly weak
require 'strict_call_weak_2.inc';

// Will succeed: Function was declared in weak mode, but that does not matter
// This file uses strict mode, so the call is strict, and float denied for int
function_declared_in_weak_mode(1.0);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: function_declared_in_weak_mode() expects argument #1 ($x) to be of type int, float given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): function_declared_in_weak_mode(1)
#1 {main}
  thrown in %sstrict_call_weak_2.inc on line 5
