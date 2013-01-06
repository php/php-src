--TEST--
gzdeflate()/gzinflate() and invalid params 
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php

var_dump(gzdeflate());
var_dump(gzdeflate("", 1000));
var_dump(gzdeflate("", -1));

var_dump(gzdeflate(""));
var_dump(gzdeflate("", 9));

$string = "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony";

var_dump($data1 = gzdeflate($string));
var_dump($data2 = gzdeflate($string, 9));

var_dump(gzinflate());
var_dump(gzinflate(""));
var_dump(gzinflate("asfwe", 1000));
var_dump(gzinflate("asdf", -1));

var_dump(gzinflate("asdf"));
var_dump(gzinflate("asdf", 9));

var_dump(gzinflate($data1));
var_dump(gzinflate($data2));
$data2{4} = 0;
var_dump(gzinflate($data2));

echo "Done\n";
?>
--EXPECTF--	
Warning: gzdeflate() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: gzdeflate(): compression level (1000) must be within -1..9 in %s on line %d
bool(false)
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"
string(%d) "%a"

Warning: gzinflate() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: gzinflate(): data error in %s on line %d
bool(false)

Warning: gzinflate(): data error in %s on line %d
bool(false)

Warning: gzinflate(): length (-1) must be greater or equal zero in %s on line %d
bool(false)

Warning: gzinflate(): data error in %s on line %d
bool(false)

Warning: gzinflate(): data error in %s on line %d
bool(false)
string(94) "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony"
string(94) "Answer me, it can't be so hard
Cry to relieve what's in your heart
Desolation, grief and agony"

Warning: gzinflate(): data error in %s on line %d
bool(false)
Done
