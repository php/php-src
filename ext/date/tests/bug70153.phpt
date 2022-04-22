--TEST--
Bug #70153 (\DateInterval incorrectly unserialized)
--FILE--
<?php
$i1 = \DateInterval::createFromDateString('+1 month');
print_r($i1);

$s = serialize($i1);
print_r($s);

echo "\n";

$i2 = unserialize($s);
print_r($i2);

var_dump($i1->days, $i2->days);
?>
--EXPECT--
DateInterval Object
(
    [from_string] => 1
    [date_string] => +1 month
)
O:12:"DateInterval":2:{s:11:"from_string";b:1;s:11:"date_string";s:8:"+1 month";}
DateInterval Object
(
    [from_string] => 1
    [date_string] => +1 month
)
bool(false)
bool(false)
