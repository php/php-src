--TEST--
Early operators mixed
--FILE--
<?php
function test1(int $v) {
    return $v = $v - 1 ?: return $v;
}
echo test1(3);
echo test1(2);
echo test1(1);
echo test1(0);
echo PHP_EOL;

function test2(int $v, int $stop) {
    while (true) {
        $v-- ?: return $v;
        $stop !== $v ?: return $v;
    }
    return 'never here';
}
echo test2(10, 3);
echo test2(10, 2);
echo test2(10, 1);
echo test2(10, 0);
echo test2(10, -5);
echo PHP_EOL;
?>
--EXPECT--
211-1
3210-1
