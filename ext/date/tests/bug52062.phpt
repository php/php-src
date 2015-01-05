--TEST--
Bug #52062 (large timestamps with DateTime::getTimestamp and DateTime::setTimestamp) (32 bit)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die('skip 32-bit only');
?>
--INI--
date.timezone=UTC
--FILE--
<?php
$d = new DateTime('@100000000000');
var_dump($d->format('Y-m-d H:i:s U'));
var_dump($d->getTimestamp());
var_dump($d->format('U'));

$d->setTimestamp(100000000000);
var_dump($d->format('Y-m-d H:i:s U'));
var_dump($d->getTimestamp());

$i = new DateInterval('PT100000000000S');
var_dump($i->format('%s'));
?>
--EXPECTF--
string(32) "5138-11-16 09:46:40 100000000000"
bool(false)
string(12) "100000000000"

Warning: DateTime::setTimestamp() expects parameter 1 to be integer, float given in %s on line %d
string(32) "5138-11-16 09:46:40 100000000000"
bool(false)
string(10) "1215752192"