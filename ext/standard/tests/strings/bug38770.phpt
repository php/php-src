--TEST--
Bug #38770 (unpack() broken with longs on 64 bit machines)
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
    [1] => -30000
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
