--TEST--
Bug #63392 (DateTime::modify() start of week inconsistency)
Description:
------------

--FILE--
<?php

$dt = new DateTime("2012-05-13");
$dt->modify("Sunday this week");
var_dump($dt->format('r'));
?>
--EXPECT--
string(31) "Sun, 13 May 2012 00:00:00 +0000"
