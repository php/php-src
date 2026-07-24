--TEST--
Live ranges should be ordered according to "start" position
--FILE--
<?php
set_error_handler(function($no, $msg) { throw new Exception; });

try {
    $a = [];
    $str = "$a${"y$a$a"}y";
} catch (Exception $e) {
}
?>
DONE
--EXPECTF--
Deprecated: Using ${expr} (variable variables) in strings is deprecated, use {${expr}} instead in %s on line %d

Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(2, 'Array to string...', '%s', %d)
#1 {main}
  thrown in %s on line %d
