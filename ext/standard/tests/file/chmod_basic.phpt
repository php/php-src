--TEST--
chmod() basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip non-windows only test');
}
?>
--FILE--
<?php

define("MODE_MASK", 07777);

$filename = __FILE__ . ".tmp";

$fd = fopen($filename, "w+");
fclose($fd);

// Not all platforms allow every mode bit on a regular file. FreeBSD, for
// example, rejects the sticky bit with EFTYPE. Detect the supported bits
// first, so that the test does not depend on platform-specific semantics.
$mode_mask = 0777;
foreach ([04000, 02000, 01000] as $bit) {
    @chmod($filename, 0777);
    clearstatcache();
    if (@chmod($filename, 0777 | $bit)) {
        clearstatcache();
        if (fileperms($filename) & $bit) {
            $mode_mask |= $bit;
        }
    }
}

for ($perms_to_set = 07777; $perms_to_set >= 0; $perms_to_set--) {
    $perms = $perms_to_set & $mode_mask;
    chmod($filename, $perms);
    clearstatcache();
    $set_perms = (fileperms($filename) & MODE_MASK);
    if ($set_perms != $perms) {
        printf("Error: %o does not match %o\n", $set_perms, $perms);
    }
}
var_dump(chmod($filename, 0777));

unlink($filename);
echo "done";

?>
--EXPECT--
bool(true)
done
