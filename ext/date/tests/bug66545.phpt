--TEST--
Bug #66545 (DateTime)
--INI--
date.timezone=Europe/Paris
--FILE--
<?php

$debut = mktime(0, 0, 0, 10, 27, 2013);
$fin = mktime(23, 59, 59, 11, 10, 2013);

$d1 = new DateTime('now',new DateTimeZone('Europe/Paris'));
$d2 = new DateTime('now',new DateTimeZone('Europe/Paris'));
$d1->setTimestamp($debut);
$d2->setTimestamp($fin);
$diff = $d1->diff($d2);
print_r($diff);
?>
--EXPECT--
DateInterval Object
(
    [y] => 0
    [m] => 0
    [d] => 14
    [h] => 23
    [i] => 59
    [s] => 59
    [f] => 0
    [invert] => 0
    [days] => 14
    [from_string] => 
)
