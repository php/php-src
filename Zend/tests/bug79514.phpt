--TEST--
Bug #79514 (Memory leaks while including unexistent file)
--FILE--
<?php
$filename = __DIR__ . '/bug79514.doesnotexist';
// approximate size of the zend_string
$size = 8 + 3 * PHP_INT_SIZE + strlen($filename) & ~(PHP_INT_SIZE - 1);
$mem1 = memory_get_usage();
for ($i = 0; $i < 100; $i++) {
    @include $filename;
}
$mem2 = memory_get_usage();
var_dump($mem2 - $mem1 < 100 * $size);
?>
--EXPECT--
bool(true)
