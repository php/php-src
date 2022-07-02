--TEST--
SPL: FixedArray: Trying to access inexistent item
--FILE--
<?php

try {
    $a = new SplFixedArray(0);
    echo $a[0]++;
} catch (Exception $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Index invalid or out of range
