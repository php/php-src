--TEST--
juliantojd()
--EXTENSIONS--
calendar
--FILE--
<?php
echo juliantojd( 0, 0,    0). "\n";
echo juliantojd( 1, 1, 1582). "\n";
echo juliantojd(10, 5, 1582). "\n";
echo juliantojd( 1, 1, 1970). "\n";
echo juliantojd( 1, 1, 2999). "\n";
echo juliantojd( 1, 1, -4713). "\n";
?>
--EXPECT--
0
2298884
2299161
2440601
2816443
0
