--TEST--
Bug #49059 (DateTime::diff() repeats previous sub() operation)
--FILE--
<?php
date_default_timezone_set('Asia/Calcutta');

$date1 = date_create("2009-03-27");
$date2 = date_create("2009-03-01");
print "\$date1 at init: " . $date1->format("Y-m-d") . "\n";
print "\$date2 at init: " . $date2->format("Y-m-d") . "\n";
$diff = $date1->diff($date2);
print "\$date1 after first diff: " . $date1->format("Y-m-d") . "\n";
print "\$diff->days after first diff: " . $diff->days . "\n";
$date1 = $date1->sub(new DateInterval("P2D"));
print "\$date1 after sub: " . $date1->format("Y-m-d") . "\n";
$diff = $date1->diff($date2);
print "\$date1 after second diff (called at \$date1): " .
$date1->format("Y-m-d") . "\n";
print "\$diff->days after second diff: " . $diff->days . "\n";
$diff = $date2->diff($date1);
print "\$date1 after third diff (called at \$date2): " .
$date1->format("Y-m-d") . "\n";
print "\$diff->days after third diff: " . $diff->days . "\n";
?>
--EXPECT--
$date1 at init: 2009-03-27
$date2 at init: 2009-03-01
$date1 after first diff: 2009-03-27
$diff->days after first diff: 26
$date1 after sub: 2009-03-25
$date1 after second diff (called at $date1): 2009-03-25
$diff->days after second diff: 24
$date1 after third diff (called at $date2): 2009-03-25
$diff->days after third diff: 24
