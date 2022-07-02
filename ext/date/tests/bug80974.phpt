--TEST--
Bug #80974: Wrong diff between 2 dates in different timezones
--FILE--
<?php
$dtToronto = new DateTime('2012-01-01 00:00:00.000000 America/Toronto');
$dtVancouver = new DateTime('2012-01-01 00:00:00.000000 America/Vancouver');

echo $dtVancouver->diff($dtToronto)->format('%h');
?>
--EXPECT--
3
