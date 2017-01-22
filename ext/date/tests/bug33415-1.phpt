--TEST--
Bug #33415 [1] (Possibly invalid non-one-hour DST or timezone shifts)
--FILE--
<?php

print "TZ=America/Jujuy  - Is it OK for this to be 2 AM, rather than 1
AM as per most DST transitions?\n";
date_default_timezone_set("America/Jujuy");
$tStamp = mktime (17, 17, 17, 1, 7593, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Monday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Monday            00:00:00\n\n";

print "TZ=Asia/Tbilisi - Is it OK for this to be 2 AM?\n";
date_default_timezone_set("Asia/Tbilisi");
$tStamp = mktime (17, 17, 17, 1, 12863, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Sunday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Sunday            00:00:00\n\n";
?>
--EXPECT--
TZ=America/Jujuy  - Is it OK for this to be 2 AM, rather than 1
AM as per most DST transitions?
tStamp=Monday 1990-10-15 17:17:17 WART 0
result=Monday 1990-10-22 00:00:00 WART 0
wanted=Monday            00:00:00

TZ=Asia/Tbilisi - Is it OK for this to be 2 AM?
tStamp=Sunday 2005-03-20 17:17:17 +03 0
result=Sunday 2005-03-27 00:00:00 +03 0
wanted=Sunday            00:00:00
