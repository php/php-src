--TEST--
Bug #81263 (Wrong result from DateTimeImmutable::diff)
--FILE--
<?php

$dt1 = new DateTimeImmutable('2020-07-19 18:30:00', new DateTimeZone('Europe/Berlin'));
$dt2 = new DateTimeImmutable('2020-07-19 16:30:00', new DateTimeZone('UTC'));

print_r($dt2->diff($dt1));
print_r($dt1->diff($dt2));
?>
--EXPECTF--
DateInterval Object
(
    [y] => 0
    [m] => 0
    [d] => 0
    [h] => 0
    [i] => 0
    [s] => 0
    [f] => 0
    [invert] => 0
    [days] => 0
    [from_string] => 
)
DateInterval Object
(
    [y] => 0
    [m] => 0
    [d] => 0
    [h] => 0
    [i] => 0
    [s] => 0
    [f] => 0
    [invert] => 0
    [days] => 0
    [from_string] => 
)
