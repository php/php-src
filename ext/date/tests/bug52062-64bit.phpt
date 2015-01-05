--TEST--
Bug #52062 (large timestamps with DateTime::getTimestamp and DateTime::setTimestamp) (64 bit)
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) die('skip 64 bit only');
?>
--INI--
date.timezone=UTC
--FILE--
<?php
$d = new DateTime('@100000000000');
var_dump($d->format('Y-m-d H:i:s U'));
var_dump($d->getTimestamp());

$d->setTimestamp(100000000000);
var_dump($d->format('Y-m-d H:i:s U'));
var_dump($d->getTimestamp());

$i = new DateInterval('PT100000000000S');
var_dump($i->format('%s'));
?>
--EXPECT--
string(32) "5138-11-16 09:46:40 100000000000"
int(100000000000)
string(32) "5138-11-16 09:46:40 100000000000"
int(100000000000)
string(12) "100000000000"
