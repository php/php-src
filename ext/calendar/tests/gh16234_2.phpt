--TEST--
GH-16234 jewishtojd overflow on year argument
--EXTENSIONS--
calendar
--FILE--
<?php
jewishtojd(10, 6, 2147483647);
echo "DONE";
?>
--EXPECTF--
DONE
