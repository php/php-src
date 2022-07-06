--TEST--
DatePeriod can be used as an IteratorAggregate
--FILE--
<?php

$period = new DatePeriod('R2/2012-07-01T00:00:00Z/P7D');
foreach ($period as $i => $date) {
    echo "$i: ", $date->format('Y-m-d'), "\n";
}

echo "\n";
foreach ($period->getIterator() as $i => $date) {
    echo "$i: ", $date->format('Y-m-d'), "\n";
}

echo "\n";
$iter = $period->getIterator();
for (; $iter->valid(); $iter->next()) {
    $i = $iter->key();
    $date = $iter->current();
    echo "$i: ", $date->format('Y-m-d'), "\n";
}

echo "\n";
$iter->rewind();
for (; $iter->valid(); $iter->next()) {
    $i = $iter->key();
    $date = $iter->current();
    echo "$i: ", $date->format('Y-m-d'), "\n";
}

echo "\n";
foreach (new IteratorIterator($period) as $i => $date) {
    echo "$i: ", $date->format('Y-m-d'), "\n";
}

// Extension that does not overwrite getIterator().
class MyDatePeriod1 extends DatePeriod {
}

echo "\n";
$period = new MyDatePeriod1('R2/2012-07-01T00:00:00Z/P7D');
foreach ($period as $i => $date) {
    echo "$i: ", $date->format('Y-m-d'), "\n";
}

// Extension that does overwrite getIterator().
class MyDatePeriod2 extends DatePeriod {
    public function getIterator(): Iterator {
        return new ArrayIterator([1, 2, 3]);
    }
}

echo "\n";
$period = new MyDatePeriod2('R2/2012-07-01T00:00:00Z/P7D');
foreach ($period as $i => $notDate) {
    echo "$i: $notDate\n";
}

?>
--EXPECT--
0: 2012-07-01
1: 2012-07-08
2: 2012-07-15

0: 2012-07-01
1: 2012-07-08
2: 2012-07-15

0: 2012-07-01
1: 2012-07-08
2: 2012-07-15

0: 2012-07-01
1: 2012-07-08
2: 2012-07-15

0: 2012-07-01
1: 2012-07-08
2: 2012-07-15

0: 2012-07-01
1: 2012-07-08
2: 2012-07-15

0: 1
1: 2
2: 3
