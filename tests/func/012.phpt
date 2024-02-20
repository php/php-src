--TEST--
Basic test for in_array function
--FILE--
<?php
echo in_array(1, [1,2,3]);
echo in_array(4, [1,2,3]);
?>
--EXPECT--
1
