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
test_open_basedir_before("symlink");

$target = ($initdir."/test/ok/ok.txt");
var_dump(symlink($target, "../bad/symlink.txt"));
var_dump(symlink($target, "../symlink.txt"));
var_dump(symlink($target, "../bad/./symlink.txt"));
var_dump(symlink($target, "./.././symlink.txt"));

$symlink = ($initdir."/test/ok/symlink.txt");
var_dump(symlink("../bad/bad.txt", $symlink));
var_dump(symlink("../bad", $symlink));
var_dump(symlink("../bad/./bad.txt", $symlink));
var_dump(symlink("../bad/bad.txt", $symlink));
var_dump(symlink("./.././bad", $symlink));

$target = ($initdir."/test/ok/ok.txt");

var_dump(symlink($target, $symlink));
var_dump(unlink($symlink));

var_dump(mkdir("ok2"));
$symlink = ($initdir."/test/ok/ok2/ok.txt");
var_dump(symlink("../ok.txt", $symlink)); // $target == (dirname($symlink)."/".$target) == ($initdir."/test/ok/ok.txt");
var_dump(unlink($symlink));

test_open_basedir_after("symlink");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [symlink] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: symlink(): open_basedir restriction in effect. File(%s/test/bad/symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: symlink(): open_basedir restriction in effect. File(%s/test/symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: symlink(): open_basedir restriction in effect. File(%s/test/bad/symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: symlink(): open_basedir restriction in effect. File(%s/test/symlink.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: symlink(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: symlink(): open_basedir restriction in effect. File(%s/test/bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: symlink(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: symlink(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: symlink(): open_basedir restriction in effect. File(%s/test/bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
*** Finished testing open_basedir configuration [symlink] ***

