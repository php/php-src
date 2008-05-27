--TEST--
Bug #44075 (strtok misbehaving)
--FILE--
<?php

$str  = '1-2';
var_dump(strtok($str, '-'));
var_dump(strtok('-'));

print "\n----\n";

$str  = '1-2-3';
$stok = strtok($str, '-');

do {
	var_dump($stok);
} while ($stok = strtok('  '));

print "\n----\n";

$str  = '1--2--3';

var_dump(strtok($str, '--'));
var_dump(strtok('--'));

while ($stok = strtok(' ')) {
	var_dump($stok);
}

print "\n----\n";

$str  = '1-  2-3';
$stok = strtok($str, '-');

do {
	var_dump($stok);
} while ($stok = strtok('  '));

print "\n----\n";

$str  = '1-  
2-3';
$stok = strtok($str, '-');

do {
	var_dump($stok);
} while ($stok = strtok('  '));

print "\n----\n";

$str  = '1- -- 2-3';
$stok = strtok($str, '-');

do {
	var_dump($stok);
} while ($stok = strtok('  '));

?>
--EXPECT--
unicode(1) "1"
unicode(1) "2"

----
unicode(1) "1"
unicode(3) "2-3"

----
unicode(1) "1"
unicode(1) "2"
unicode(2) "-3"

----
unicode(1) "1"
unicode(3) "2-3"

----
unicode(1) "1"
unicode(4) "
2-3"

----
unicode(1) "1"
unicode(2) "--"
unicode(3) "2-3"
