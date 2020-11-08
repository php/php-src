--TEST--
Bug #79514 (Memory leaks while including unexistent file)
--FILE--
<?php
$mem1 = memory_get_usage(true);
for ($i = 0; $i < 100000; $i++) {
    @include __DIR__ . '/bug79514.doesnotexist';
}
$mem2 = memory_get_usage(true);
var_dump($mem2 - $mem1 < 100000);
?>
--EXPECT--
bool(true)
