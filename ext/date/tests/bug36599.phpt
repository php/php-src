--TEST--
Bug #36599 (DATE_W3C format constant incorrect).
--FILE--
<?php
date_default_timezone_set("UTC");

echo date( DATE_ATOM, strtotime( "2006-03-03 08:47:55" ) ), "\n";
echo date( DATE_W3C,  strtotime( "2006-03-03 08:47:55" ) ), "\n";
?>
--EXPECT--
2006-03-03T08:47:55+00:00
2006-03-03T08:47:55+00:00
