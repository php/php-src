--TEST--
Bug #14561 (strtotime() bug)
--FILE--
<?php
putenv("TZ=GMT");
echo strtotime("19:30 Dec 17"), "\n";
echo strtotime("Dec 17 19:30"), "\n";
?>
--EXPECT--
1134847800
1134847800
