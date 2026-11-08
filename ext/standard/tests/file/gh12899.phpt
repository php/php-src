--TEST--
GH-12899 (copy() target is blank in PHP 8.3.0)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die('skip Linux only');
if (!is_readable('/proc/version')) die('skip /proc/version is not readable');
?>
--FILE--
<?php
/* procfs files report a size of 0, so copy_file_range() can return 0 for them */
$src = '/proc/version';
$dest = __DIR__ . '/gh12899_dest.txt';
$expected = file_get_contents($src);
var_dump(strlen($expected) > 0);

var_dump(copy($src, $dest));
var_dump(file_get_contents($dest) === $expected);

$in = fopen($src, 'r');
$out = fopen($dest, 'w');
var_dump(stream_copy_to_stream($in, $out) === strlen($expected));
fclose($in);
fclose($out);
var_dump(file_get_contents($dest) === $expected);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh12899_dest.txt');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
