--TEST--
fstab - /etc/fstab entries
--SKIPIF--
<?php if (!function_exists('fstab')) die("skip");
?>
--FILE--
<?php
$entries = fstab();

if ($entries === false) {
    echo "fstab failed.\n";
} else {
    $cur = $entries[0];
    echo "has fs_spec entry " . (isset($cur["fs_spec"]) && strlen($cur["fs_spec"])) . "\n";
    echo "has fs_file entry " . (isset($cur["fs_file"]) && strlen($cur["fs_file"])) . "\n";
    echo "has fs_vfstype entry " . (isset($cur["fs_vfstype"]) && strlen($cur["fs_vfstype"])) . "\n";
    echo "has fs_mntops entry " . (isset($cur["fs_mntops"]) && strlen($cur["fs_mntops"])) . "\n";
    echo "has fs_type entry " . (isset($cur["fs_type"]) && strlen($cur["fs_type"])) . "\n";
    echo "has fs_freq entry " . (isset($cur["fs_freq"])) . "\n";
    echo "has fs_passno entry " . (isset($cur["fs_passno"]));
}
?>
--EXPECT--
has fs_spec entry 1
has fs_file entry 1
has fs_vfstype entry 1
has fs_mntops entry 1
has fs_type entry 1
has fs_freq entry 1
has fs_passno entry 1
