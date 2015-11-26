--TEST--
gzencode()
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php
$original = str_repeat("hallo php",4096);
$packed = gzencode($original);
echo strlen($packed)." ".strlen($original). "\n";
if (strcmp($original, gzdecode($packed)) == 0) echo "Strings are equal";
?>
--EXPECT--
118 36864
Strings are equal
