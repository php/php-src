--TEST--
Comparison of DateComparableInterval
--FILE--
<?php

$d1 = new DateTime('2019-04-01');
$d2 = new DateTime('2019-05-01');
$d3 = new DateTime('2019-06-01');
$d4 = new DateTime('2019-07-01');
$i1 = $d1->diff($d2);
$i2 = $d2->diff($d3);
$i3 = $d3->diff($d4);

// $i1, $i2 and $i3 are all one month long, however
// $i1 and $i3 have 30 days, while $i2 has 31.
var_dump($i1 <=> $i2);
var_dump($i2 <=> $i3);
var_dump($i1 <=> $i3);

?>
--EXPECT--
int(-1)
int(1)
int(0)
