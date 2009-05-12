--TEST--
Bug #33532 (Different output for strftime() and date())
--INI--
error_reporting=2047
date.timezone=UTC
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
strftime 10:00:00 AM July 1 2005 EST +1000
datestr  10:00:00 AM July 1 2005 EST
