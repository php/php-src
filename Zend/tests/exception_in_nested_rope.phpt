--TEST--
Exception during nested rope
--FILE--
<?php

set_error_handler(function() { throw new Exception; });

try {
    $a = "foo";
    $str = "$a${"y$a$a"}y";
} catch (Exception $e) {
    echo "Exception\n";
}

?>
--EXPECTF--
Deprecated: Using ${expr} (variable variables) in strings is deprecated, use {${expr}} instead in %s on line %d

Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(2, 'Undefined varia...', '%s', %d)
#1 {main}
  thrown in %s on line %d
