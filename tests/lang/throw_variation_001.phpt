--TEST--
Catching an exception thrown from an included file
--FILE--
<?php

try {
    include "inc_throw.inc";
} catch (Exception $e) {
    echo "caught exception\n";
}

?>
--EXPECT--
caught exception
