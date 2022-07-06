--TEST--
Bug #30532 (strtotime - crossing daylight savings time)
--INI--
date.timezone=America/New_York
--FILE--
<?php

echo date('Y-m-d H:i:s T', strtotime('2004-10-31 EDT +1 hour'))."\n";
echo date('Y-m-d H:i:s T', strtotime('2004-10-31 EDT +2 hours'))."\n";
echo date('Y-m-d H:i:s T', strtotime('2004-10-31 EDT +3 hours'))."\n";

echo "\n";

echo date('Y-m-d H:i:s T', strtotime('2004-10-31 +1 hour'))."\n";
echo date('Y-m-d H:i:s T', strtotime('2004-10-31 +2 hours'))."\n";
echo date('Y-m-d H:i:s T', strtotime('2004-10-31 +3 hours'))."\n";
?>
--EXPECT--
2004-10-31 01:00:00 EDT
2004-10-31 01:00:00 EST
2004-10-31 02:00:00 EST

2004-10-31 01:00:00 EDT
2004-10-31 02:00:00 EST
2004-10-31 03:00:00 EST
