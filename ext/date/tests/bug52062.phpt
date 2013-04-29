--TEST--
Bug #52062 (large timestamps with DateTime::getTimestamp and DateTime::setTimestamp) (32 bit)
--SKIPIF--
<?php
if (PHP_INT_SIZE == 8) die('skip 32-bit only');
?>
--XFAIL--
Waiting for resolution of the 32-bit case.
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
--EXPECT--
string(32) "5138-11-16 09:46:40 100000000000"
bool(false)
string(12) "100000000000"
string(30) "2008-07-11 04:56:32 1215752192"
int(1215752192)
string(10) "1215752192"
