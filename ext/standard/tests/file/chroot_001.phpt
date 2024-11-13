--TEST--
chroot()
--SKIPIF--
<?php
if (!function_exists("chroot")) {
    die("skip chroot() not available");
}
// Skip if not being run by root (files are always readable, writeable and executable)
$filename = @tempnam(__DIR__, 'root_check_');
if (!file_exists($filename)) {
    die('WARN Unable to create the "root check" file');
}

$isRoot = fileowner($filename) == 0;

unlink($filename);

if (!$isRoot) {
    die('SKIP Must be run as root');
}
?>
--FILE--
<?php
mkdir("chroot_001_x");
var_dump(is_dir("chroot_001_x"));
var_dump(chroot("chroot_001_x"));
var_dump(is_dir("chroot_001_x"));
var_dump(realpath("."));
?>
--CLEAN--
<?php
rmdir("chroot_001_x");
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
string(1) "/"
