--TEST--
Live range & return from finally
--FILE--
<?php
$array = [1];
foreach ([0] as $_) {
    foreach ($array as $v) {
        try {
            echo "ok\n";
            return;
        } finally {
            echo "ok\n";
            return;
        }
    }
}
?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d
ok
ok