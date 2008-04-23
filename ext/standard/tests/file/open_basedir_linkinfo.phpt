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
test_open_basedir_before("linkinfo", FALSE);
$directory = dirname(__FILE__);

chdir($directory);

$target = ($directory."/test/bad/bad.txt");
$symlink = ($directory."/test/ok/symlink.txt");
var_dump(symlink($target, $symlink));

chdir($directory."/test/ok");

var_dump(linkinfo("symlink.txt"));
var_dump(linkinfo("../ok/symlink.txt"));
var_dump(linkinfo("../ok/./symlink.txt"));
var_dump(linkinfo("./symlink.txt"));
var_dump(linkinfo($directory."/test/ok/symlink.txt"));

$target = ($directory."/test/ok/ok.txt");
$symlink = ($directory."/test/ok/symlink.txt");
var_dump(symlink($target, $symlink));
var_dump(linkinfo($symlink));
var_dump(unlink($symlink));

test_open_basedir_after("linkinfo");
?>
--CLEAN--
<?php
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

Warning: unlink(): open_basedir restriction in effect. File(%s/test/ok/symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)
*** Finished testing open_basedir configuration [linkinfo] ***

