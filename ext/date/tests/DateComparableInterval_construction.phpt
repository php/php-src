--TEST--
Ensure no invalid DateComparableIntervals can be constructed
--FILE--
<?php

try {
    new DateComparableInterval("P1M");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$payloads = [
'O:22:"DateComparableInterval":16:{s:1:"y";i:0;s:1:"m";i:0;s:1:"d";i:6;s:1:"h";i:0;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";d:0;s:7:"weekday";i:0;s:16:"weekday_behavior";i:0;s:17:"first_last_day_of";i:0;s:6:"invert";i:0;s:4:"days";i:-99999;s:12:"special_type";i:0;s:14:"special_amount";i:0;s:21:"have_weekday_relative";i:0;s:21:"have_special_relative";i:0;}',
'O:22:"DateComparableInterval":16:{s:1:"y";i:0;s:1:"m";i:0;s:1:"d";i:6;s:1:"h";i:0;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";d:0;s:7:"weekday";i:0;s:16:"weekday_behavior";i:0;s:17:"first_last_day_of";i:0;s:6:"invert";i:0;s:4:"days";i:6;s:12:"special_type";i:0;s:14:"special_amount";i:0;s:21:"have_weekday_relative";i:1;s:21:"have_special_relative";i:0;}',
'O:22:"DateComparableInterval":16:{s:1:"y";i:0;s:1:"m";i:0;s:1:"d";i:6;s:1:"h";i:0;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";d:0;s:7:"weekday";i:0;s:16:"weekday_behavior";i:0;s:17:"first_last_day_of";i:0;s:6:"invert";i:0;s:4:"days";i:6;s:12:"special_type";i:0;s:14:"special_amount";i:0;s:21:"have_weekday_relative";i:0;s:21:"have_special_relative";i:1;}',
];
foreach ($payloads as $payload) {
    try {
        unserialize($payload);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

try {
    DateComparableInterval::__set_state(array(
       'y' => 0,
       'm' => 0,
       'd' => 6,
       'h' => 0,
       'i' => 0,
       's' => 0,
       'f' => 0,
       'weekday' => 0,
       'weekday_behavior' => 0,
       'first_last_day_of' => 0,
       'invert' => 0,
       'days' => -99999,
       'special_type' => 0,
       'special_amount' => 0,
       'have_weekday_relative' => 0,
       'have_special_relative' => 0,
    ));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Call to private DateComparableInterval::__construct() from invalid context
Malformed comparable interval
Malformed comparable interval
Malformed comparable interval
Malformed comparable interval
