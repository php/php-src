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
?>
--EXPECT--
1
2
42
di Object
(
    [y] => 0
    [m] => 0
    [d] => 11
    [h] => 0
    [i] => 0
    [s] => 0
    [f] => 0
    [invert] => 0
    [days] => 
    [from_string] => 
    [date_string] => 
    [unit] => 42
)
