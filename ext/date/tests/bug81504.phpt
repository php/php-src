--TEST--
Bug #81504: Incorrect timezone transition details for POSIX data
--INI--
date.timezone=UTC
--FILE--
<?php

$tz = new DateTimeZone('Europe/Amsterdam');
foreach ($tz->getTransitions(strtotime("1996-01-01"), strtotime("1997-12-31")) as $tr) {
    echo "{$tr['time']} {$tr['offset']} {$tr['abbr']}\n";
}
?>
--EXPECT--
1996-01-01T00:00:00+00:00 3600 CET
1996-03-31T01:00:00+00:00 7200 CEST
1996-10-27T01:00:00+00:00 3600 CET
1997-03-30T01:00:00+00:00 7200 CEST
1997-10-26T01:00:00+00:00 3600 CET
