--TEST--
Bug #63392 (DateTime::modify() start of week inconsistency)
Description:
------------
Calling $dt->modify("Monday this week") for all dates 13¨C19 May 2012 (Sun¨CSat) 
result in the same date (2012-05-14), suggesting that "this week" runs Sun¨CSat. 
However, calling $dt->modify("Sunday this week") gives the "next" Sunday (2012-
05-20) for the same range.

On a related note, the documentation on relative date formats is lacking on the 
behaviour of this and strtotime when used with locales; one might expect the 
start of the week to be interpreted from LC_TIME.
--FILE--
<?php

$dt = new DateTime("2012-05-13");
$dt->modify("Sunday this week");
var_dump($dt->format('r')
?>
--EXPECT--
string(31) "Sun, 13 May 2012 00:00:00 +0000"
