--TEST--
gregoriantojd()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
echo gregoriantojd( 0, 0,    0). "\n";
echo gregoriantojd( 1, 1, 1582). "\n";
echo gregoriantojd(10, 5, 1582). "\n";
echo gregoriantojd( 1, 1, 1970). "\n";
echo gregoriantojd( 1, 1, 2999). "\n";
echo gregoriantojd( 1, 1, -4714). "\n";
echo gregoriantojd( 11, 24, -4714). "\n";
?>
--EXPECT--
0
2298874
2299151
2440588
2816423
0
0
