--TEST--
gzdeflate()/gzinflate()
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
$original = str_repeat("hallo php",4096);
$packed=gzdeflate($original);
echo strlen($packed)." ".strlen($original)."\n";
$unpacked=gzinflate($packed);
if (strcmp($original,$unpacked)==0) echo "Strings are equal";

echo "\n";
$original = 'aaaaaaaaaaaaaaa';
$packed=gzdeflate($original);
echo strlen($packed)." ".strlen($original)."\n";
$unpacked=gzinflate($packed);
if (strcmp($original,$unpacked)==0) echo "Strings are equal";
?>
--EXPECT--
100 36864
Strings are equal
5 15
Strings are equal