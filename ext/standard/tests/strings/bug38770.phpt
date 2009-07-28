--TEST--
Bug #38770 (unpack() broken with longs on 64 bit machines)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

foreach (array('N','I','l') as $v) {
	print_r(unpack($v, pack($v, -30000)));
}

echo "Done\n";
?>
--EXPECT--	
Array
(
    [1] => 4294937296
)
Array
(
    [1] => -30000
)
Array
(
    [1] => -30000
)
Done
