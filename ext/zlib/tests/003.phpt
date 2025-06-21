--TEST--
gzencode()
--EXTENSIONS--
zlib
--FILE--
<?php
$original = str_repeat("hallo php",4096);
$packed = gzencode($original);
echo strlen($packed)." ".strlen($original). "\n";
if ($original === gzdecode($packed)) echo "Strings are equal\n";
?>
--EXPECT--
118 36864
Strings are equal
