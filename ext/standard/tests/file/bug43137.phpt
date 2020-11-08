--TEST--
Bug #43137 (rmdir() and rename() do not clear statcache)
--FILE--
<?php
    $toname = "TO_" . md5(microtime());
    $dirname = "FROM_" . md5(microtime());

    mkdir($dirname);
    var_dump(is_dir($dirname)); // Expected: true
    rename($dirname, $toname);
    var_dump(is_dir($dirname)); // Expected: false
    var_dump(is_dir($toname)); // Expected: true
    rmdir($toname);
    var_dump(is_dir($toname)); // Expected: false
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
