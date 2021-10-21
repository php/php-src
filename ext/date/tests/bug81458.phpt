--TEST--
Bug #81458: Incorrect difference after timezone change
--FILE--
<?php

$first = (new DateTime('2018-07-01 00:00:00.000000 America/Toronto'))
    ->setTimezone(new DateTimeZone('UTC'));
$second = new DateTime('2018-07-02 00:00:00.000000 America/Toronto');

var_dump($first->diff($second)->days);
var_dump($first->diff($second)->d);
?>
--EXPECT--
int(1)
int(1)
