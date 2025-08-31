--TEST--
Bug #62326 (date_diff() function returns false result)
--FILE--
<?php
date_default_timezone_set("Europe/Berlin");

$start_timestamp    = date_create('2012-06-01');
$end_timestamp      = date_create('2012-12-01');
$difference = date_diff($start_timestamp, $end_timestamp);

echo $difference->format('%mM / %dD %hH %iM'), "\n";
?>
--EXPECT--
6M / 0D 0H 0M
