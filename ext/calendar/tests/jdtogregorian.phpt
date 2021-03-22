--TEST--
jdtogregorian()
--EXTENSIONS--
calendar
--FILE--
<?php
echo jdtogregorian(0). "\n";
echo jdtogregorian(1). "\n";
echo jdtogregorian(2298874). "\n";
echo jdtogregorian(2299151). "\n";
echo jdtogregorian(2440588). "\n";
echo jdtogregorian(2816423). "\n";
?>
--EXPECT--
0/0/0
11/25/-4714
1/1/1582
10/5/1582
1/1/1970
1/1/2999
