--TEST--
Bug #45081 (strtotime incorrectly interprets SGT time zone)
--INI--
date.timezone=Asia/Singapore
--FILE--
<?php

print strtotime('2008-05-23 00:00:00 +08');
print "\n";
print strtotime('2008-05-23 00:00:00');

?>
--EXPECTF--
1211472000
1211472000
