--TEST--
Bug #46587 (mt_rand() does not check that max is greater than min).
--FILE--
<?php

var_dump(mt_rand(3,8));
var_dump(mt_rand(8,3));

echo "Done.\n";
?>
--EXPECTF--
int(%d)

Warning: mt_rand(): max(3) is smaller than min(8) in %s on line %d
bool(false)
Done.
