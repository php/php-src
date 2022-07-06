--TEST--
Bug #32270 (strtotime/date behavior)
--INI--
date.timezone=America/Los_Angeles
--FILE--
<?php

echo date("m/d/Y H:i:s T", -2145888000)."\n";

echo strtotime("Jan 1 1902")."\n";

echo date("m/d/Y H:i:s T", -631123200)."\n";

echo strtotime("Jan 1 1950")."\n";

echo date("m/d/Y H:i:s T", 946713600)."\n";

echo strtotime("Jan 1 2000")."\n";
?>
--EXPECT--
01/01/1902 00:00:00 PST
-2145888000
01/01/1950 00:00:00 PST
-631123200
01/01/2000 00:00:00 PST
946713600
