--TEST--
Bug #70153 (\DateInterval incorrectly unserialized)
--FILE--
<?php
$i1 = \DateInterval::createFromDateString('+1 month');
print_r($i1);

$s = serialize($i1);
print_r($s);

$i2 = unserialize($s);
print_r($i2);

var_dump($i1->days, $i2->days);
?>
--EXPECT--
DateInterval Object
(
    [y] => 0
    [m] => 1
    [d] => 0
    [h] => 0
    [i] => 0
    [s] => 0
    [f] => 0
    [invert] => 0
    [days] => 
    [have_special_relative] => 0
)
O:12:"DateInterval":16:{s:1:"y";i:0;s:1:"m";i:1;s:1:"d";i:0;s:1:"h";i:0;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";d:0;s:7:"weekday";i:0;s:16:"weekday_behavior";i:0;s:17:"first_last_day_of";i:0;s:6:"invert";i:0;s:4:"days";b:0;s:12:"special_type";i:0;s:14:"special_amount";i:0;s:21:"have_weekday_relative";i:0;s:21:"have_special_relative";i:0;}DateInterval Object
(
    [y] => 0
    [m] => 1
    [d] => 0
    [h] => 0
    [i] => 0
    [s] => 0
    [f] => 0
    [invert] => 0
    [days] => 
    [have_special_relative] => 0
)
bool(false)
bool(false)
