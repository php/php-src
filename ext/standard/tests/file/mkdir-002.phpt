--TEST--
mkdir(dir, 0777) tests
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
?>
--FILE--
<?php

var_dump(mkdir("testdir", 0777));
var_dump(mkdir("testdir/subdir", 0777));
var_dump(`ls -l testdir`);
var_dump(rmdir("testdir/subdir"));
var_dump(rmdir("testdir"));

var_dump(mkdir("./testdir", 0777));
var_dump(mkdir("./testdir/subdir", 0777));
var_dump(`ls -l ./testdir`);
var_dump(rmdir("./testdir/subdir"));
var_dump(rmdir("./testdir"));

var_dump(mkdir(dirname(__FILE__)."/testdir", 0777));
var_dump(mkdir(dirname(__FILE__)."/testdir/subdir", 0777));
$dirname = dirname(__FILE__)."/testdir";
var_dump(`ls -l $dirname`);
var_dump(rmdir(dirname(__FILE__)."/testdir/subdir"));
var_dump(rmdir(dirname(__FILE__)."/testdir"));

echo "Done\n";
?>
--EXPECTF--	
bool(true)
bool(true)
string(%d) "%s
d%s subdir
"
bool(true)
bool(true)
bool(true)
bool(true)
string(%d) "%s
d%s subdir
"
bool(true)
bool(true)
bool(true)
bool(true)
string(%d) "%s
d%s subdir
"
bool(true)
bool(true)
Done
