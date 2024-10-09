--TEST--
GH-16235 (jdtogregorian overflow on argument)
--EXTENSIONS--
calendar
--FILE--
<?php
jdtogregorian(536838867);
echo "DONE";
?>
--EXPECT--
DONE
