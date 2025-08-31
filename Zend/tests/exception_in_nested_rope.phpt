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
Exception
