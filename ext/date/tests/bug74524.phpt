--TEST--
Bug #74524 Date diff is bad calculated, in same time zone
--FILE--
<?php

$a = new DateTime("2017-11-17 22:05:26.000000");
$b = new DateTime("2017-04-03 22:29:15.079459");

$diff = $a->diff($b);
echo $diff->h;
?>
--EXPECT--
23
