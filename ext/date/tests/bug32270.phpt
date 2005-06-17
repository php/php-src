--TEST--
Bug #32270 (strtotime/date behavior)
--FILE--
<?php
putenv("TZ=America/New_York");

echo date("m/d/Y H:i:s", -2145888000)."\n";

echo strtotime("Jan 1 1902")."\n";

echo date("m/d/Y H:i:s", -631123200)."\n";

echo strtotime("Jan 1 1950")."\n";

echo date("m/d/Y H:i:s", 946713600)."\n";

echo strtotime("Jan 1 2000")."\n";
?>
--EXPECT--
01/01/1902 00:00:00
-2145888000
01/01/1950 00:00:00
-631123200
01/01/2000 00:00:00
946713600
