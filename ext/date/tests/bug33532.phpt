--TEST--
Bug #33532 (Different output for strftime() and date())
--INI--
error_reporting=2047
date.timezone=UTC
--SKIPIF--
<?php
if(PHP_OS == 'Darwin' || defined('PHP_WINDOWS_VERSION_MAJOR')) die("skip strftime uses system TZ on Darwin and Windows");
if (!@strftime('%Z')) die('skip strftime does not support %Z');
?>
--FILE--
<?php

setlocale(LC_ALL, 'C');

print "TZ has NOT been set\n";
print "Should strftime==datestr? Strftime seems to assume GMT tStamp.\n";
$input = "10:00:00 AM July 1 2005";
print "input    " . $input . "\n";
$tStamp = strtotime($input);
print "strftime " . strftime("%r %B%e %Y %Z %z", $tStamp) . "\n";
print "datestr  " . date ("H:i:s A F j Y T", $tStamp) . "\n";

print "\nSetting TZ\n";
date_default_timezone_set('Australia/Sydney');
putenv("TZ=Australia/Sydney");
$input = "10:00:00 AM July 1 2005";
print "input    " . $input . "\n";
$tStamp = strtotime($input);
print "strftime " . strftime("%r %B%e %Y %Z %z", $tStamp) . "\n";
print "datestr  " . date ("H:i:s A F j Y T", $tStamp) . "\n";

?>
--EXPECT--
TZ has NOT been set
Should strftime==datestr? Strftime seems to assume GMT tStamp.
input    10:00:00 AM July 1 2005
strftime 10:00:00 AM July 1 2005 UTC +0000
datestr  10:00:00 AM July 1 2005 UTC

Setting TZ
input    10:00:00 AM July 1 2005
strftime 10:00:00 AM July 1 2005 AEST +1000
datestr  10:00:00 AM July 1 2005 AEST
