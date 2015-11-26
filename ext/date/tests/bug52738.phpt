--TEST--
Bug #52738 (Can't use new properties in class extended from DateInterval)
--FILE--
<?php
class di extends DateInterval {
    public $unit = 1;
}

$I = new di('P10D');
echo $I->unit."\n";
$I->unit++;
echo $I->unit."\n";
$I->unit = 42;
echo $I->unit."\n";
$I->d++;
print_r($I);
--EXPECT--
1
2
42
di Object
(
    [unit] => 42
    [y] => 0
    [m] => 0
    [d] => 11
    [h] => 0
    [i] => 0
    [s] => 0
    [weekday] => 0
    [weekday_behavior] => 0
    [first_last_day_of] => 0
    [invert] => 0
    [days] => 
    [special_type] => 0
    [special_amount] => 0
    [have_weekday_relative] => 0
    [have_special_relative] => 0
)
