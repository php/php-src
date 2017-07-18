--TEST--
Bug #33414 [2] (Comprehensive list of incorrect days returned after strotime() / date() tests)
--FILE--
<?php
print "TZ=Pacific/Rarotonga - wrong day.\n";
date_default_timezone_set("Pacific/Rarotonga");
$tStamp = mktime (17, 17, 17, 1, 1, 1970);       
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Tuesday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Tuesday            00:00:00\n\n"; 

print "TZ=Atlantic/South_Georgia - wrong day.\n";
date_default_timezone_set("Atlantic/South_Georgia");
$tStamp = mktime (17, 17, 17, 1, 1, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Tuesday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Tuesday            00:00:00\n\n";

print "TZ=America/Port-au-Prince - wrong day.\n";
date_default_timezone_set("America/Port-au-Prince");
$tStamp = mktime (17, 17, 17, 1, 12871, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Monday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Monday            00:00:00\n\n";

print "TZ=Pacific/Enderbury - wrong day, off by 2 days.\n";
date_default_timezone_set("Pacific/Enderbury");
$tStamp = mktime (17, 17, 17, 1, 1, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Monday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Monday            00:00:00\n\n";

print "TZ=Pacific/Kiritimati - wrong day, off by 2 days.\n";
date_default_timezone_set("Pacific/Kiritimati");
$tStamp = mktime (17, 17, 17, 1, 1, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Monday", $tStamp);                 
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Monday            00:00:00\n\n";

print "TZ=America/Managua - wrong day.\n";     
date_default_timezone_set("America/Managua");
$tStamp = mktime (17, 17, 17, 1, 12879, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Tuesday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Tuesday            00:00:00\n\n";     

print "TZ=Pacific/Pitcairn - wrong day.\n";
date_default_timezone_set("Pacific/Pitcairn");
$tStamp = mktime (17, 17, 17, 1, 1, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Wednesday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Wednesday            00:00:00\n\n";

print "TZ=Pacific/Fakaofo - wrong day.\n";
date_default_timezone_set("Pacific/Fakaofo");
$tStamp = mktime (17, 17, 17, 1, 1, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Saturday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Saturday            00:00:00\n\n";

print "TZ=Pacific/Johnston - wrong day.\n";
date_default_timezone_set("Pacific/Johnston");
$tStamp = mktime (17, 17, 17, 1, 1, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Friday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Friday            00:00:00\n\n";
?>
--EXPECT--
TZ=Pacific/Rarotonga - wrong day.
tStamp=Thursday 1970-01-01 17:17:17 -1030 0
result=Tuesday 1970-01-06 00:00:00 -1030 0
wanted=Tuesday            00:00:00

TZ=Atlantic/South_Georgia - wrong day.
tStamp=Thursday 1970-01-01 17:17:17 -02 0
result=Tuesday 1970-01-06 00:00:00 -02 0
wanted=Tuesday            00:00:00

TZ=America/Port-au-Prince - wrong day.
tStamp=Monday 2005-03-28 17:17:17 EST 0
result=Monday 2005-04-04 00:00:00 EDT 1
wanted=Monday            00:00:00

TZ=Pacific/Enderbury - wrong day, off by 2 days.
tStamp=Thursday 1970-01-01 17:17:17 -12 0
result=Monday 1970-01-05 00:00:00 -12 0
wanted=Monday            00:00:00

TZ=Pacific/Kiritimati - wrong day, off by 2 days.
tStamp=Thursday 1970-01-01 17:17:17 -1040 0
result=Monday 1970-01-05 00:00:00 -1040 0
wanted=Monday            00:00:00

TZ=America/Managua - wrong day.
tStamp=Tuesday 2005-04-05 17:17:17 CST 0
result=Tuesday 2005-04-12 00:00:00 CDT 1
wanted=Tuesday            00:00:00

TZ=Pacific/Pitcairn - wrong day.
tStamp=Thursday 1970-01-01 17:17:17 -0830 0
result=Wednesday 1970-01-07 00:00:00 -0830 0
wanted=Wednesday            00:00:00

TZ=Pacific/Fakaofo - wrong day.
tStamp=Thursday 1970-01-01 17:17:17 -11 0
result=Saturday 1970-01-03 00:00:00 -11 0
wanted=Saturday            00:00:00

TZ=Pacific/Johnston - wrong day.
tStamp=Thursday 1970-01-01 17:17:17 HST 0
result=Friday 1970-01-02 00:00:00 HST 0
wanted=Friday            00:00:00
