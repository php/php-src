--TEST--
chroot()
--SKIPIF--
<?php
if (!function_exists("chroot")) {
    die("skip chroot() not available");
}
if (getenv('SKIP_REPEAT')) {
    die('skip the changed root persists across repetitions');
}
if (!@chroot('/')) {
    die('skip chroot is unavailable for this process');
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
