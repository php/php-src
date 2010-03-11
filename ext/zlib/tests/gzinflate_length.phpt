--TEST--
gzinflate() and $length argument
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php
$original = 'aaaaaaaaaaaaaaa';
$packed=gzdeflate($original);
echo strlen($packed)." ".strlen($original)."\n";
$unpacked=gzinflate($packed, strlen($original));
if (strcmp($original,$unpacked)==0) echo "Strings are equal\n";

$unpacked=gzinflate($packed, strlen($original)*10);
if (strcmp($original,$unpacked)==0) echo "Strings are equal\n";

$unpacked=gzinflate($packed, 1);
if ($unpacked === false) echo "Failed (as expected)\n";
?>
--EXPECTF--
5 15
Strings are equal
Strings are equal

Warning: gzinflate(): insufficient memory in %s on line %d
Failed (as expected)

