--TEST--
Can call internal functions from global constants
--FILE--
<?php
const VALUES = RANGE(1, 100);
const SUM = array_sum(VALUES);

echo "SUM is " . SUM . "\n";
?>
--EXPECT--
SUM is 5050
