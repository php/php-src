--TEST--
GH-21992: symlink() must not resolve the final dangling link component
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    require_once __DIR__ . '/windows_links/common.inc';
    skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
}
?>
--FILE--
<?php
$dir = __DIR__ . '/gh21992';
$targetDir = $dir . '/target_dir';

// dangling link to nonexisting $ghost
$deadLink = $dir . '/dead_link.txt';
$ghost = $targetDir . '/ghost.txt';
$newTarget = $dir . '/nothing';

// real link to $realFile
$realLink = $dir . '/real_link.txt';
$realFile = $targetDir . '/real.txt';
$realTarget = $dir . '/something';

mkdir($dir);
mkdir($targetDir);
touch($realFile); // ensure $realFile exists

// verify the real file works fine
var_dump(symlink($realFile, $realLink));
var_dump(readlink($realLink));
var_dump(symlink($realTarget, $realLink));
var_dump(is_link($realLink));
var_dump(readlink($realLink));
var_dump(is_link($realFile));

// verify the ghost file works as intended
var_dump(symlink($ghost, $deadLink));
var_dump(readlink($deadLink));
var_dump(symlink($newTarget, $deadLink));
var_dump(is_link($deadLink));
var_dump(readlink($deadLink));
var_dump(is_link($ghost));
?>
--CLEAN--
<?php
$dir = __DIR__ . '/gh21992';
$targetDir = $dir . '/target_dir';
$deadLink = $dir . '/dead_link.txt';
$ghost = $targetDir . '/ghost.txt';
$realLink = $dir . '/real_link.txt';
$realFile = $targetDir . '/real.txt';

@unlink($ghost);
@unlink($deadLink);
@unlink($realLink);
@unlink($realFile);
@rmdir($targetDir);
@rmdir($dir);
?>
--EXPECTF--
bool(true)
string(%d) "%s%egh21992%etarget_dir%ereal.txt"

Warning: symlink(): File exists in %s on line %d
bool(false)
bool(true)
string(%d) "%s%egh21992%etarget_dir%ereal.txt"
bool(false)
bool(true)
string(%d) "%s%egh21992%etarget_dir%eghost.txt"

Warning: symlink(): Dangling symlink at %s on line %d
bool(false)
bool(true)
string(%d) "%s%egh21992%etarget_dir%eghost.txt"
bool(false)
