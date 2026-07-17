--TEST--
Exception during rope finalization
--FILE--
<?php

set_error_handler(function() { throw new Exception; });

try {
    $b = "foo";
    $str = "y$b$a";
} catch (Exception $e) {
    echo "Exception\n";
}

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(2, 'Undefined varia...', '%s', %d)
#1 {main}
  thrown in %s on line %d
