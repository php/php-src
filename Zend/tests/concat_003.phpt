--TEST--
Concatenating many small strings should not slowdown allocations
--SKIPIF--
<?php if (PHP_DEBUG) { die ("skip debug version is slow"); } ?>
--FILE--
<?php

$time = microtime(TRUE);

/* This might vary on Linux/Windows, so the worst case and also count in slow machines. */
$t_max = 1.0;

$datas = array_fill(0, 220000, [
    '000.000.000.000',
    '000.255.255.255',
    '保留地址',
    '保留地址',
    '保留地址',
    '保留地址',
    '保留地址',
    '保留地址',
]);

$time = microtime(TRUE);
$texts = '';
foreach ($datas AS $data)
{
    $texts .= implode("\t", $data) . "\r\n";
}

$t = microtime(TRUE) - $time;
var_dump($t < $t_max);

?>
+++DONE+++
--EXPECT--
bool(true)
+++DONE+++
