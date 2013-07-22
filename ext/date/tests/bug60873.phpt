--TEST--
Bug #60873 (some inspections of DateTime member variables cause creation, can break asserts)
--INI--
date.timezone=GMT
--FILE--
<?php
$a = new DateTime('2010-01-01 08:45:00', new DateTimeZone('UTC'));
$str = $a->format(DateTime::ISO8601);
$b = new DateTime($str, new DateTimeZone('UTC'));

echo "\n";
echo "a->timezone_type: " . $a->timezone_type . "\n";
echo "b->timezone_type: " . $b->timezone_type . "\n";

echo "\na:   " . print_r($a, true) . "\n";
echo "\nstr: $str\n";
echo "b:   " . print_r($b, true) . "\n";

echo "a->timezone_type: " . $a->timezone_type . "\n";
echo "b->timezone_type: " . $b->timezone_type . "\n";

$eq = ($a == $b);
echo "\na == b: $eq\n";
?>
--EXPECTF--
a->timezone_type: 3
b->timezone_type: 1

a:   DateTime Object
(
    [timezone_type] => 3
    [timezone] => UTC
    [date] => 2010-01-01 08:45:00
)


str: 2010-01-01T08:45:00+0000
b:   DateTime Object
(
    [timezone_type] => 1
    [timezone] => +00:00
    [date] => 2010-01-01 08:45:00
)

a->timezone_type: 3
b->timezone_type: 1

a == b: 1