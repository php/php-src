--TEST--
Test open_basedir configuration
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
?>
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("readlink", FALSE);

chdir($initdir);

$target = ($initdir."/test/bad/bad.txt");
$symlink = ($initdir."/test/ok/symlink.txt");
var_dump(symlink($target, $symlink));

chdir($initdir."/test/ok");

var_dump(readlink("symlink.txt"));
var_dump(readlink("../ok/symlink.txt"));
var_dump(readlink("../ok/./symlink.txt"));
var_dump(readlink("./symlink.txt"));
var_dump(readlink($initdir."/test/ok/symlink.txt"));

$target = ($initdir."/test/ok/ok.txt");
$symlink = ($initdir."/test/ok/symlink.txt");
var_dump(symlink($target, $symlink));
var_dump(readlink($symlink));
var_dump(unlink($symlink));

test_open_basedir_after("readlink");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [readlink] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: readlink(): open_basedir restriction in effect. File(symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: readlink(): open_basedir restriction in effect. File(../ok/symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: readlink(): open_basedir restriction in effect. File(../ok/./symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: readlink(): open_basedir restriction in effect. File(./symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: readlink(): open_basedir restriction in effect. File(%s/test/ok/symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: symlink(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: readlink(): open_basedir restriction in effect. File(%s/test/ok/symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: unlink(): open_basedir restriction in effect. File(%s/test/ok/symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)
*** Finished testing open_basedir configuration [readlink] ***

