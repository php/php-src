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

var_dump(mkdir("mkdir-002", 0777));
var_dump(mkdir("mkdir-002/subdir", 0777));
var_dump(`ls -l mkdir-002`);
var_dump(rmdir("mkdir-002/subdir"));
var_dump(rmdir("mkdir-002"));

var_dump(mkdir("./mkdir-002", 0777));
var_dump(mkdir("./mkdir-002/subdir", 0777));
var_dump(`ls -l ./mkdir-002`);
var_dump(rmdir("./mkdir-002/subdir"));
var_dump(rmdir("./mkdir-002"));

var_dump(mkdir(dirname(__FILE__)."/mkdir-002", 0777));
var_dump(mkdir(dirname(__FILE__)."/mkdir-002/subdir", 0777));
$dirname = dirname(__FILE__)."/mkdir-002";
var_dump(`ls -l $dirname`);
var_dump(rmdir(dirname(__FILE__)."/mkdir-002/subdir"));
var_dump(rmdir(dirname(__FILE__)."/mkdir-002"));

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
