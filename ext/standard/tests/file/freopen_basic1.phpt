--TEST--
basic freopen() test 
--FILE--
<?php

$line1 = "First line\n";
$line2 = "Second line\n";

$filename1 = dirname(__FILE__)."/freopen_basic1.tmp1";
$filename2 = dirname(__FILE__)."/freopen_basic1.tmp2";

$fd = fopen($filename1, "w");
//1st line goes into the 1st file
fwrite($fd, $line1);
freopen($filename2, "w", $fd);
//2nd line goes into the 2nd file
fwrite($fd, $line2);

echo "---\n";
echo file_get_contents($filename1);
echo "---\n";
echo file_get_contents($filename2);
echo "---\n";

?>
--CLEAN--
<?php
$filename1 = dirname(__FILE__)."/freopen_basic1.tmp1";
$filename2 = dirname(__FILE__)."/freopen_basic1.tmp2";
unlink($filename1);
unlink($filename2);
?>
--EXPECT--
---
First line
---
Second line
---
