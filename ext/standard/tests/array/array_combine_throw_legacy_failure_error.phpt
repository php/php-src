--TEST--
Test array_combine() function : error conditions - with throw_legacy_failure declare statement
--FILE--
<?php

declare(throw_legacy_failure=1);

try {
    array_combine(array(1, 2), array(1, 2, 3));
} catch (\ErrorException $e) {
    echo "Error caught\n";
}
?>
DONE
--EXPECTF--
Error caught
DONE
