--TEST--
GH-21992 (symlink() on dangling symlink returns true and creates link at wrong location)
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die('skip not for Windows');
}
?>
--FILE--
<?php
$dir = __DIR__ . '/gh21992';
$targetDir = $dir . '/target_dir';
$ghost = $targetDir . '/ghost.txt';
$deadLink = $dir . '/dead_link.txt';
$realFile = $dir . '/real.txt';

mkdir($dir);
mkdir($targetDir);
touch($realFile);

// Create a dangling link
var_dump(symlink($ghost, $deadLink));
var_dump(is_link($deadLink));

echo "--- symlink() over dangling link\n";
var_dump(symlink($realFile, $deadLink));
var_dump(readlink($deadLink) === $ghost);
var_dump(file_exists($ghost) || is_link($ghost));

echo "--- link() over dangling link\n";
var_dump(link($realFile, $deadLink));
var_dump(readlink($deadLink) === $ghost);
var_dump(file_exists($ghost) || is_link($ghost));

echo "--- relative link path\n";
chdir($dir);
var_dump(symlink('real.txt', 'dead_link.txt'));
var_dump(file_exists($ghost) || is_link($ghost));
?>
--CLEAN--
<?php
$dir = __DIR__ . '/gh21992';
@unlink($dir . '/target_dir/ghost.txt');
@unlink($dir . '/dead_link.txt');
@unlink($dir . '/real.txt');
@rmdir($dir . '/target_dir');
@rmdir($dir);
?>
--EXPECTF--
bool(true)
bool(true)
--- symlink() over dangling link

Warning: symlink(): File exists in %s on line %d
bool(false)
bool(true)
bool(false)
--- link() over dangling link

Warning: link(): File exists in %s on line %d
bool(false)
bool(true)
bool(false)
--- relative link path

Warning: symlink(): File exists in %s on line %d
bool(false)
bool(false)
