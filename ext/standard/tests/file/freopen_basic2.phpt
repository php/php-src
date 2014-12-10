--TEST--
basic freopen() test with errors 
--FILE--
<?php

$line1 = "First line\n";
$line2 = "Second line\n";

$filename1 = dirname(__FILE__)."/freopen_basic2.tmp1";
$filename2 = dirname(__FILE__)."/freopen_basic2.tmp2";
$fd = fopen($filename1, "w");
fwrite($fd, $line1);
freopen($filename2, "r", $fd);
fwrite($fd, $line2);

echo "---\n";
echo file_get_contents($filename1);
echo "---\n";
echo file_get_contents($filename2);
echo "---\n";

?>
--CLEAN--
<?php
$filename1 = dirname(__FILE__)."/freopen_basic2.tmp1";
$filename2 = dirname(__FILE__)."/freopen_basic2.tmp2";
unlink($filename1);
unlink($filename2);
?>
--EXPECTF--
Warning: freopen(): No such file or directory in %s on line %d
---
First line
Second line
---

Warning: file_get_contents(%sfreopen_basic2.tmp2): failed to open stream: No such file or directory in %s on line %d
---
