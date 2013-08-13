--TEST--
Bug #30069 (floats as strings used in calculations do not work)
--FILE--
<?php
echo ".1" * "2";
echo "\n";
echo "-.1" * "2";
echo "\n";
?>
--EXPECT--
0.2
-0.2
