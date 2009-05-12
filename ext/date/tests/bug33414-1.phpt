--TEST--
Bug #33414 [1] (Comprehensive list of incorrect days returned after strotime() / date() tests)
--INI--
date.timezone=America/Mendoza
--FILE--
<?php 

print "TZ=America/Mendoza - wrong day.\n";
$tStamp = mktime (17, 17, 17, 1, 8327, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Sunday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Sunday              00:00:00\n\n";

print "TZ=America/Catamarca - wrong day.\n";
date_default_timezone_set("America/Catamarca");
$tStamp = mktime (17, 17, 17, 1, 7599, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Sunday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Sunday              00:00:00\n\n";

print "TZ=America/Cordoba - wrong day.\n";
date_default_timezone_set("America/Cordoba");
$tStamp = mktime (17, 17, 17, 1, 7599, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Sunday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Sunday              00:00:00\n\n";

print "TZ=America/Rosario - wrong day.\n";
date_default_timezone_set("America/Rosario");
$tStamp = mktime (17, 17, 17, 1, 7958, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Tuesday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Tuesday           00:00:00\n\n";

print "TZ=Europe/Vienna - wrong day - giving unexpected results, at
least on my system :-)\n";
date_default_timezone_set("Europe/Vienna");
$tStamp = mktime (17, 17, 17, 1, 3746, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Thursday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Thursday             00:00:00\n\n";

print "TZ=Asia/Baku - wrong day.\n";
date_default_timezone_set("Asia/Baku");
$tStamp = mktime (17, 17, 17, 1, 8299, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Sunday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Sunday              00:00:00\n\n";

print "TZ=America/Noronha - wrong day.\n";
date_default_timezone_set("America/Noronha");
$tStamp = mktime (17, 17, 17, 1, 10866, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Friday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Friday              00:00:00\n\n";

print "TZ=America/Havana - wrong day.\n";
date_default_timezone_set("America/Havana");
$tStamp = mktime (17, 17, 17, 1, 12720, 1970);  
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Thursday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Thursday             00:00:00\n\n";

print "TZ=Europe/Tallinn - wrong day.\n";
date_default_timezone_set("Europe/Tallinn");   
$tStamp = mktime (17, 17, 17, 1, 11777, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Saturday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Saturday          00:00:00\n\n";  

print "TZ=Asia/Jerusalem - wrong day.\n";     
date_default_timezone_set("Asia/Jerusalem");
$tStamp = mktime (17, 17, 17, 1, 13056, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Thursday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Thursday             00:00:00\n\n";         

print "TZ=Europe/Vilnius - wrong day.\n";
date_default_timezone_set("Europe/Vilnius");
$tStamp = mktime (17, 17, 17, 1, 12140, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Friday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Friday            00:00:00\n\n";

print "TZ=Pacific/Kwajalein - wrong day.\n";
date_default_timezone_set("Pacific/Kwajalein");
$tStamp = mktime (17, 17, 17, 1, 8626, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Friday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Friday            00:00:00\n\n";

print "TZ=Asia/Ulan_Bator - wrong day.\n";
date_default_timezone_set("Asia/Ulan_Bator");
$tStamp = mktime (17, 17, 17, 1, 11588, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Saturday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Saturday            00:00:00\n\n";

print "TZ=America/Cancun - wrong day.\n";
date_default_timezone_set("America/Cancun");
$tStamp = mktime (17, 17, 17, 1, 11785, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Sunday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Sunday            00:00:00\n\n";

print "TZ=America/Mexico_City - wrong day.\n";
date_default_timezone_set("America/Mexico_City");
$tStamp = mktime (17, 17, 17, 1, 11781, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Wednesday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Wednesday            00:00:00\n\n";

print "TZ=America/Mazatlan - wrong day.\n";
date_default_timezone_set("America/Mazatlan");
$tStamp = mktime (17, 17, 17, 1, 11780, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Tuesday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Tuesday            00:00:00\n\n";

print "TZ=America/Chihuahua - wrong day.\n";
date_default_timezone_set("America/Chihuahua");
$tStamp = mktime (17, 17, 17, 1, 11782, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Thursday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Thursday            00:00:00\n\n";

print "TZ=Asia/Kuala_Lumpur - wrong day.\n";     
date_default_timezone_set("Asia/Kuala_Lumpur");
$tStamp = mktime (17, 17, 17, 1, 4380, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Monday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Monday            00:00:00\n\n";            

print "TZ=Pacific/Chatham - wrong day.\n";       
date_default_timezone_set("Pacific/Chatham");  
$tStamp = mktime (17, 17, 17, 1, 1762, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Monday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Monday            00:00:00\n\n";            

print "TZ=America/Lima - wrong day.\n";        
date_default_timezone_set("America/Lima");   
$tStamp = mktime (17, 17, 17, 1, 5839, 1970); 
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Thursday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Thursday            00:00:00\n\n";          

print "TZ=Asia/Karachi - wrong day.\n";
date_default_timezone_set("Asia/Karachi");
$tStamp = mktime (17, 17, 17, 1, 11783, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Friday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Friday            00:00:00\n\n";

print "TZ=America/Asuncion - wrong day.\n";
date_default_timezone_set("America/Asuncion");
$tStamp = mktime (17, 17, 17, 1, 11746, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Wednesday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Wednesday            00:00:00\n\n";

print "TZ=Asia/Singapore - wrong day.\n";
date_default_timezone_set("Asia/Singapore");
$tStamp = mktime (17, 17, 17, 1, 4383, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Thursday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Thursday             00:00:00\n\n";

print "TZ=America/Montevideo - wrong day.\n";
date_default_timezone_set("America/Montevideo");
$tStamp = mktime (17, 17, 17, 1, 12678, 1970);
print "tStamp=". date("l Y-m-d H:i:s T I", $tStamp). "\n";
$strtotime_tstamp = strtotime("next Thursday", $tStamp);
print "result=".date("l Y-m-d H:i:s T I", $strtotime_tstamp)."\n";
print "wanted=Thursday             00:00:00\n\n";

?>
--EXPECT--
TZ=America/Mendoza - wrong day.
tStamp=Sunday 1992-10-18 17:17:17 ARST 1
result=Sunday 1992-10-25 00:00:00 ARST 1
wanted=Sunday              00:00:00

TZ=America/Catamarca - wrong day.
tStamp=Sunday 1990-10-21 17:17:17 ARST 1
result=Sunday 1990-10-28 00:00:00 ARST 1
wanted=Sunday              00:00:00

TZ=America/Cordoba - wrong day.
tStamp=Sunday 1990-10-21 17:17:17 ARST 1
result=Sunday 1990-10-28 00:00:00 ARST 1
wanted=Sunday              00:00:00

TZ=America/Rosario - wrong day.
tStamp=Tuesday 1991-10-15 17:17:17 WART 0
result=Tuesday 1991-10-22 00:00:00 ARST 1
wanted=Tuesday           00:00:00

TZ=Europe/Vienna - wrong day - giving unexpected results, at
least on my system :-)
tStamp=Thursday 1980-04-03 17:17:17 CET 0
result=Thursday 1980-04-10 00:00:00 CEST 1
wanted=Thursday             00:00:00

TZ=Asia/Baku - wrong day.
tStamp=Sunday 1992-09-20 17:17:17 AZST 1
result=Sunday 1992-09-27 00:00:00 AZT 0
wanted=Sunday              00:00:00

TZ=America/Noronha - wrong day.
tStamp=Friday 1999-10-01 17:17:17 FNT 0
result=Friday 1999-10-08 00:00:00 FNST 1
wanted=Friday              00:00:00

TZ=America/Havana - wrong day.
tStamp=Thursday 2004-10-28 17:17:17 CDT 1
result=Thursday 2004-11-04 00:00:00 CDT 1
wanted=Thursday             00:00:00

TZ=Europe/Tallinn - wrong day.
tStamp=Saturday 2002-03-30 17:17:17 EET 0
result=Saturday 2002-04-06 00:00:00 EEST 1
wanted=Saturday          00:00:00

TZ=Asia/Jerusalem - wrong day.
tStamp=Thursday 2005-09-29 17:17:17 IDT 1
result=Thursday 2005-10-06 00:00:00 IDT 1
wanted=Thursday             00:00:00

TZ=Europe/Vilnius - wrong day.
tStamp=Friday 2003-03-28 17:17:17 EET 0
result=Friday 2003-04-04 00:00:00 EEST 1
wanted=Friday            00:00:00

TZ=Pacific/Kwajalein - wrong day.
tStamp=Friday 1993-08-13 17:17:17 KWAT 0
result=Saturday 1993-08-21 00:00:00 MHT 0
wanted=Friday            00:00:00

TZ=Asia/Ulan_Bator - wrong day.
tStamp=Saturday 2001-09-22 17:17:17 ULAST 1
result=Saturday 2001-09-29 00:00:00 ULAST 1
wanted=Saturday            00:00:00

TZ=America/Cancun - wrong day.
tStamp=Sunday 2002-04-07 17:17:17 CDT 1
result=Sunday 2002-04-14 00:00:00 CDT 1
wanted=Sunday            00:00:00

TZ=America/Mexico_City - wrong day.
tStamp=Wednesday 2002-04-03 17:17:17 CST 0
result=Wednesday 2002-04-10 00:00:00 CDT 1
wanted=Wednesday            00:00:00

TZ=America/Mazatlan - wrong day.
tStamp=Tuesday 2002-04-02 17:17:17 MST 0
result=Tuesday 2002-04-09 00:00:00 MDT 1
wanted=Tuesday            00:00:00

TZ=America/Chihuahua - wrong day.
tStamp=Thursday 2002-04-04 17:17:17 MST 0
result=Thursday 2002-04-11 00:00:00 MDT 1
wanted=Thursday            00:00:00

TZ=Asia/Kuala_Lumpur - wrong day.
tStamp=Monday 1981-12-28 17:17:17 MALT 0
result=Monday 1982-01-04 00:00:00 MYT 0
wanted=Monday            00:00:00

TZ=Pacific/Chatham - wrong day.
tStamp=Monday 1974-10-28 17:17:17 CHAST 0
result=Monday 1974-11-04 00:00:00 CHADT 1
wanted=Monday            00:00:00

TZ=America/Lima - wrong day.
tStamp=Thursday 1985-12-26 17:17:17 PET 0
result=Thursday 1986-01-02 00:00:00 PEST 1
wanted=Thursday            00:00:00

TZ=Asia/Karachi - wrong day.
tStamp=Friday 2002-04-05 17:17:17 PKT 0
result=Friday 2002-04-12 00:00:00 PKST 1
wanted=Friday            00:00:00

TZ=America/Asuncion - wrong day.
tStamp=Wednesday 2002-02-27 17:17:17 PYST 1
result=Wednesday 2002-03-06 00:00:00 PYST 1
wanted=Wednesday            00:00:00

TZ=Asia/Singapore - wrong day.
tStamp=Thursday 1981-12-31 17:17:17 SGT 0
result=Thursday 1982-01-07 00:00:00 SGT 0
wanted=Thursday             00:00:00

TZ=America/Montevideo - wrong day.
tStamp=Thursday 2004-09-16 17:17:17 UYT 0
result=Thursday 2004-09-23 00:00:00 UYST 1
wanted=Thursday             00:00:00
