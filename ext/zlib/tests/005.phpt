--TEST--
gzcompress()/gzuncompress() and invalid params
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php

var_dump(gzcompress());
var_dump(gzcompress("", 1000));
var_dump(gzcompress("", -1));

var_dump(gzcompress(""));
var_dump(gzcompress("", 9));

$string = "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony";

var_dump($data1 = gzcompress($string));
var_dump($data2 = gzcompress($string, 9));

var_dump(gzuncompress());
var_dump(gzuncompress("", 1000));
var_dump(gzuncompress("", -1));

var_dump(gzuncompress(""));
var_dump(gzuncompress("", 9));

var_dump(gzuncompress($data1));
var_dump(gzuncompress($data2));
$data2[4] = 0;
var_dump(gzuncompress($data2));

echo "Done\n";
?>
--EXPECTF--
Warning: gzcompress() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: gzcompress(): compression level (1000) must be within -1..9 in %s on line %d
bool(false)
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"

Warning: gzuncompress() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: gzuncompress(): %s error in %s on line %d
bool(false)

Warning: gzuncompress(): length (-1) must be greater or equal zero in %s on line %d
bool(false)

Warning: gzuncompress(): %s error in %s on line %d
bool(false)

Warning: gzuncompress(): %s error in %s on line %d
bool(false)
string(94) "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony"
string(94) "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony"

Warning: gzuncompress(): %s error in %s on line %d
bool(false)
Done
