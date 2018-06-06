--TEST--
date_modify() function [1]
--SKIPIF--
<?php if (!function_exists('date_create')) echo "SKIP"; ?>
--FILE--
<?php
date_default_timezone_set("Pacific/Kwajalein");
$ts = date_create("Thu Aug 19 1993 23:59:59");
echo date_format($ts, 'D, d M Y H:i:s T'), "\n";
$ts->modify("+1 second");
echo date_format($ts, 'D, d M Y H:i:s T'), "\n";

date_default_timezone_set("Europe/Amsterdam");
$ts = date_create("Sun Mar 27 01:59:59 2005");
echo date_format($ts, 'D, d M Y H:i:s T'), "\n";
$ts->modify("+1 second");
echo date_format($ts, 'D, d M Y H:i:s T'), "\n";

$ts = date_create("Sun Oct 30 01:59:59 2005");
echo date_format($ts, 'D, d M Y H:i:s T'), "\n";
$ts->modify("+ 1 hour 1 second");
echo date_format($ts, 'D, d M Y H:i:s T'), "\n";
?>
--EXPECT--
Thu, 19 Aug 1993 23:59:59 -12
Sat, 21 Aug 1993 00:00:00 +12
Sun, 27 Mar 2005 01:59:59 CET
Sun, 27 Mar 2005 03:00:00 CEST
Sun, 30 Oct 2005 01:59:59 CEST
Sun, 30 Oct 2005 03:00:00 CET
