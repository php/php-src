--TEST--
Bug #20382 [1] (strtotime ("Monday", $date) produces wrong result on DST changeover)
--INI--
date.timezone=Europe/Amsterdam
--FILE--
<?php
	$tStamp = mktime (17, 17, 17, 10, 27, 2004);
	echo "tStamp=". date("l Y-m-d H:i:s T", $tStamp). "\n";
	
	$strtotime_timestamp = strtotime ("Monday", $tStamp);
	echo "result=". date("l Y-m-d H:i:s T", $strtotime_timestamp). "\n";
	echo "wanted=Monday 2004-11-01 00:00:00 CET\n";
?>
--EXPECT--
tStamp=Wednesday 2004-10-27 17:17:17 CEST
result=Monday 2004-11-01 00:00:00 CET
wanted=Monday 2004-11-01 00:00:00 CET
