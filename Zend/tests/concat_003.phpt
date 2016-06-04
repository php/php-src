--TEST--
Concatenating many small strings should not slowdown allocations
--SKIPIF--
<?php if (PHP_DEBUG) { die ("skip debug version is slow"); } ?>
--INI--
memory_limit=256m
--FILE--
<?php

/* To note is that memory usage can vary depending on whether opcache is on. The actual
  measuring that matters is timing here. */

$time = microtime(TRUE);

/* This might vary on Linux/Windows, so the worst case and also count in slow machines. */
$t0_max = 0.1;
$t1_max = 0.4;

$datas = [];
for ($i = 0; $i < 220000; $i++)
{
    $datas[] = [
        '000.000.000.000',
        '000.255.255.255',
        '保留地址',
        '保留地址',
        '保留地址',
        '保留地址',
        '保留地址',
        '保留地址',
    ];
}

$t0 = microtime(TRUE) - $time;
var_dump($t0 < $t0_max);


$texts = '';
foreach ($datas AS $data)
{
    $texts .= implode("\t", $data) . "\r\n";
}

$t1 = microtime(TRUE) - $time;
var_dump($t1 < $t1_max);

?>
+++DONE+++
--EXPECT--
bool(true)
bool(true)
+++DONE+++
