--TEST--
Bug #67247 (spl_fixedarray_resize integer overflow)
--FILE--
<?php
$ar = new SplFixedArray(1);
echo "size: ".$ar->getSize()."\n";
try {
    $ar->setSize((PHP_INT_SIZE==8)?0x2000000000000001:0x40000001);
} catch (MemoryError $e) {
    echo $e->getMessage() . "\n";
}
echo "size: ".$ar->getSize()."\n";
?>
--EXPECT--
size: 1
The resulting SplFixedArray is too large to fit in the configured memory limit
size: 1
