--TEST--
Bug #74639 Cloning DatePeriod leads to segfault
--FILE--
<?php

$start = new DateTime('2017-05-22 09:00:00');
$end = new DateTime('2017-08-24 18:00:00');
$interval = $start->diff($end);

$period = new DatePeriod($start, $interval, $end);
$clonedPeriod = clone $period;
$clonedInterval = clone $interval;

if ($period->getStartDate() != $clonedPeriod->getStartDate()) {
    echo "failure\n";
}

if ($period->getEndDate() != $clonedPeriod->getEndDate()) {
    echo "failure\n";
}

if ($period->getDateInterval() != $clonedPeriod->getDateInterval()) {
    echo "failure\n";
}

if ($interval->format('Y-m-d H:i:s') != $clonedInterval->format('Y-m-d H:i:s')) {
    echo "failure\n";
}

echo 'success';
?>
--EXPECT--
success
