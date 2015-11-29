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
--EXPECT--
Exception
