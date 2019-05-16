--TEST--
Bug #73942: $date->modify('Friday this week') doesn't return a Friday if $date is a Sunday
--FILE--
<?php
$date1 = "2017-01-08"; // this is a Sunday
$date = new \DateTime($date1);
$date->modify('Friday this week');
$dateFormat = $date->format('Y-m-d');
echo $dateFormat, "\n";
?>
--EXPECT--
2017-01-06
