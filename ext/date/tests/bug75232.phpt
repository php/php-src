--TEST--
Bug #75232: print_r of DateTime creating side-effect
--INI--
date.timezone=UTC
--FILE--
<?php

$d1 = DateTime::createFromFormat("Ymd\THis\Z", '20170920T091600Z');
echo $d1->date, "\n";

$d2 = DateTime::createFromFormat("Ymd\THis\Z", '20170920T091600Z');
print_r($d2);
echo $d2->date, "\n";

?>
--EXPECTF--
Warning: Undefined property: DateTime::$date in %s on line %d

DateTime Object
(
    [date] => 2017-09-20 09:16:00.000000
    [timezone_type] => 3
    [timezone] => UTC
)

Warning: Undefined property: DateTime::$date in %s on line %d
