--TEST--
SPL: FixedArray: Trying to access inexistent item
--FILE--
<?php

try {
    $a = new SplFixedArray(NULL);
    echo $a[0]++;
} catch (Exception $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Index invalid or out of range
