--TEST--
GH-16258 (jddayofweek overflow on argument)
--EXTENSIONS--
calendar
--FILE--
<?php
jddayofweek(9223372036854775807, 1);
jddayofweek(-9223372036854775806, 1);
echo "DONE";
?>
--EXPECT--
DONE
