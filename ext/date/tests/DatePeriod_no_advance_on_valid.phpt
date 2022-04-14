--TEST--
Date Period iterators do not advance on valid()
--FILE--
<?php

$start = DateTime::createFromFormat('Y-m-d', '2022-01-01');
$end = DateTime::createFromFormat('Y-m-d', '2022-01-04');
$interval = DateInterval::createFromDateString('1 day');
$period = new DatePeriod($start, $interval, $end);
$iterator = $period->getIterator();

foreach ($iterator as $item) {
    echo $item->format('Y-m-d') . "\n";
}

echo "---------STEP 2\n";

foreach ($iterator as $item) {
    $iterator->valid();
    echo $item->format('Y-m-d') . "\n";
}

$period = new DatePeriod($start, $interval, $end, DatePeriod::EXCLUDE_START_DATE);
$iterator = $period->getIterator();

echo "---------STEP 3\n";

foreach ($iterator as $item) {
    echo $item->format('Y-m-d') . "\n";
}

echo "---------STEP 4\n";

foreach ($iterator as $item) {
    $iterator->valid();
    echo $item->format('Y-m-d') . "\n";
}
?>
--EXPECT--
2022-01-01
2022-01-02
2022-01-03
---------STEP 2
2022-01-01
2022-01-02
2022-01-03
---------STEP 3
2022-01-02
2022-01-03
---------STEP 4
2022-01-02
2022-01-03
