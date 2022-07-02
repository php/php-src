--TEST--
Bug #68503 (date_diff on two dates with timezone set localised returns wrong results)
--FILE--
<?php
date_default_timezone_set('Europe/London');
echo date_diff(new DateTime("2015-02-01"), new DateTime("2015-05-01"))->format( '%yY %mM %dD' ), "\n";
date_default_timezone_set('UTC');
echo date_diff(new DateTime("2015-02-01"), new DateTime("2015-05-01"))->format( '%yY %mM %dD' ), "\n";
?>
--EXPECT--
0Y 3M 0D
0Y 3M 0D
