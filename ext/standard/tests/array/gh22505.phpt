--TEST--
GH-22505 : range() reports the correct calculated size when exceeding HT_MAX_SIZE
--INI--
memory_limit=20G
--SKIPIF--
<?php
//if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
//if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
?>
--FILE--
<?php
try {
    $arr = range(0, (1 << 30) - 1);
    unset($arr);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    range(0, (1 << 30));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
echo 'Done';
?>
--EXPECT--
The supplied range exceeds the maximum array size by 1 elements: start=0, end=1073741824, step=1. Calculated size: 1073741825. Maximum size: 1073741824.
Done

