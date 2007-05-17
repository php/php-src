--TEST--
notices when incorporating arrays into strings
--FILE--
<?php
$arr = array(1,2);

echo "$arr\n";
?>
--EXPECTF--
Notice: Array to string conversion in %s on line %d
Array
