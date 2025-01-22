--TEST--
GH-16234 jewishtojd overflow on year argument
--EXTENSIONS--
calendar
--FILE--
<?php
jewishtojd(1218182888, 1, 1218182888);
echo "DONE";
?>
--EXPECT--
DONE
