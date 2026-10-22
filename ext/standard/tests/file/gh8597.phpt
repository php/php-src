--TEST--
GH-8597: rename() warnings name the missing source or destination directory
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
?>
--FILE--
<?php
$base = __DIR__ . '/gh8597';
@mkdir($base);
$src = $base . '/exists.txt';
file_put_contents($src, 'x');

echo "missing source:\n";
var_dump(rename($base . '/no-such-source.txt', $base . '/dest.txt'));

echo "missing destination directory:\n";
var_dump(rename($src, $base . '/no-such-dir/dest.txt'));

echo "success:\n";
var_dump(rename($src, $base . '/renamed.txt'));
var_dump(file_exists($base . '/renamed.txt'));
?>
--CLEAN--
<?php
$base = __DIR__ . '/gh8597';
@unlink($base . '/exists.txt');
@unlink($base . '/renamed.txt');
@unlink($base . '/dest.txt');
@rmdir($base);
?>
--EXPECTF--
missing source:

Warning: rename(): No such file or directory (source "%s/gh8597/no-such-source.txt") in %s on line %d
bool(false)
missing destination directory:

Warning: rename(): No such file or directory (destination directory "%s/gh8597/no-such-dir") in %s on line %d
bool(false)
success:
bool(true)
bool(true)
