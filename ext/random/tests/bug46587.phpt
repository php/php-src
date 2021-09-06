--TEST--
Bug #46587 (mt_rand() does not check that max is greater than min).
--FILE--
<?php

var_dump(mt_rand(3,8));
try {
    var_dump(mt_rand(8,3));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done.\n";
?>
--EXPECTF--
int(%d)
mt_rand(): Argument #2 ($max) must be greater than or equal to argument #1 ($min)
Done.
