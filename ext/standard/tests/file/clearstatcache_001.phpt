--TEST--
clearstatcache() optional parameters
--SKIPIF--
<?php
if (strncmp(PHP_OS, "WIN", 3) === 0) {
    die('skip not for Windows');
}
?>
--FILE--
<?php

@rmdir(__FILE__ . "_dir1");
@rmdir(__FILE__ . "_dir2");
@unlink(__FILE__ . "_link1");
@unlink(__FILE__ . "_link2");

mkdir(__FILE__ . "_dir1");
mkdir(__FILE__ . "_dir2");
symlink(__FILE__ . "_link1", __FILE__ . "_link2");
symlink(__FILE__ . "_dir1", __FILE__ . "_link1");

var_dump(realpath(__FILE__ . "_link2"));
passthru("rm -f " . escapeshellarg(__FILE__ . "_link1"));
var_dump(realpath(__FILE__ . "_link2"));
clearstatcache(false);
var_dump(realpath(__FILE__ . "_link2"));
clearstatcache(true, "/foo/bar");
var_dump(realpath(__FILE__ . "_link2"));
clearstatcache(true, __FILE__ . "_link2");
clearstatcache(true, __FILE__ . "_link1");
var_dump(realpath(__FILE__ . "_link2"));

@rmdir(__FILE__ . "_dir1");
@rmdir(__FILE__ . "_dir2");
@unlink(__FILE__ . "_link1");
@unlink(__FILE__ . "_link2");
?>
--EXPECTF--
%unicode|string%(%d) "%s_dir1"
%unicode|string%(%d) "%s_dir1"
%unicode|string%(%d) "%s_dir1"
%unicode|string%(%d) "%s_dir1"
bool(false)
