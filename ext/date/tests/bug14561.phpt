--TEST--
Bug #14561 (strtotime() bug)
--INI--
date.timezone=GMT
--FILE--
<?php
echo strtotime("19:30 Dec 17 2005"), "\n";
echo strtotime("Dec 17 19:30 2005"), "\n";
?>
--EXPECT--
1134847800
1134847800
