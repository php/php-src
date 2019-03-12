--TEST--
Bug #77097 (DateTime::diff gives wrong diff when the actual diff is less than 1 second)
--FILE--
<?php
$now = new DateTime('2018-11-03 11:34:20.781751');
$ago = new DateTime('2018-11-03 11:34:20.000000');

$diff = $now->diff($ago);
var_dump($diff->invert, $diff->s, $diff->f);

$diff = $ago->diff($now);
var_dump($diff->invert, $diff->s, $diff->f);

$diff = $now->diff($ago, true);
var_dump($diff->invert, $diff->s, $diff->f);

$diff = $ago->diff($now, true);
var_dump($diff->invert, $diff->s, $diff->f);
?>
--EXPECT--
int(1)
int(0)
float(0.781751)
int(0)
int(0)
float(0.781751)
int(0)
int(0)
float(0.781751)
int(0)
int(0)
float(0.781751)

