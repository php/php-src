--TEST--
Test open_basedir configuration
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
?>
--XFAIL--
BUG: open_basedir cannot delete symlink to prohibited file. See also
bugs 48111 and 52176.
--FILE--
<?php
chdir(__DIR__);
ini_set("open_basedir", ".");
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("linkinfo", FALSE);

chdir($initdir);

$target = ($initdir."/test/bad/bad.txt");
$symlink = ($initdir."/test/ok/symlink.txt");
var_dump(symlink($target, $symlink));

chdir($initdir."/test/ok");

var_dump(linkinfo("symlink.txt"));
var_dump(linkinfo("../ok/symlink.txt"));
var_dump(linkinfo("../ok/./symlink.txt"));
var_dump(linkinfo("./symlink.txt"));
var_dump(linkinfo($initdir."/test/ok/symlink.txt"));

$target = ($initdir."/test/ok/ok.txt");
$symlink = ($initdir."/test/ok/symlink.txt");
var_dump(symlink($target, $symlink));
var_dump(linkinfo($symlink));
var_dump(unlink($symlink));

test_open_basedir_after("linkinfo");
?>
--CLEAN--
<?php
chdir(__DIR__);
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [linkinfo] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)

Warning: symlink(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)
int(%d)
bool(true)
*** Finished testing open_basedir configuration [linkinfo] ***

