--TEST--
GH-16258 (jddayofweek overflow on argument)
--EXTENSIONS--
calendar
--FILE--
<?php
jddayofweek(PHP_INT_MAX, 1);
jddayofweek(PHP_INT_MIN, 1);
echo "DONE";
?>
--EXPECT--
DONE
