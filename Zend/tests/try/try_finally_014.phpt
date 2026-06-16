--TEST--
Break 2 in try and return in finally inside nested loop
--FILE--
<?php

function foo() {
    $array = [1, 2, $n = 3];
    foreach ($array as $value) {
        foreach ($array as $value) {
            try {
                echo "try\n";
                break 2;
            } finally {
                echo "finally\n";
                return;
            }
        }
    }
}

foo();
?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d
try
finally