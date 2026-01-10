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
try {
    var_dump($d->getTimestamp());
} catch (\DateRangeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($d->format('U'));

try {
    $d->setTimestamp(100000000000);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($d->format('Y-m-d H:i:s U'));
try {
    var_dump($d->getTimestamp());
} catch (\DateRangeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$i = new DateInterval('PT100000000000S');
var_dump($i->format('%s'));
?>
--EXPECT--
string(32) "5138-11-16 09:46:40 100000000000"
DateRangeError: Epoch doesn't fit in a PHP integer
string(12) "100000000000"
TypeError: DateTime::setTimestamp(): Argument #1 ($timestamp) must be of type int, float given
string(32) "5138-11-16 09:46:40 100000000000"
DateRangeError: Epoch doesn't fit in a PHP integer
string(10) "1215752192"
