--TEST--
jewishtojd()
--EXTENSIONS--
calendar
--FILE--
<?php
echo jewishtojd(-1,-1,-1). "\n";
echo jewishtojd(0,0,0). "\n";
echo jewishtojd(1,1,1). "\n";
echo jewishtojd(2,22,5763). "\n";
?>
--EXPECT--
0
0
347998
2452576
