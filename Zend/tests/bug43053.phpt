--TEST--
Bug #43053 (Regression: some numbers shown in scientific notation)
--FILE--
<?php
echo 1200000.00."\n";
echo 1300000.00."\n";
echo 1400000.00."\n";
echo 1500000.00."\n";
?>
--EXPECT--
1200000
1300000
1400000
1500000
