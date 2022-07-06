--TEST--
Test open_basedir configuration
--SKIPIF--
<?php
if(PHP_OS_FAMILY === "Windows") {
    die('skip no links on Windows');
}
?>
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
$initdir = getcwd();
test_open_basedir_before("link");

$target = ($initdir."/test/ok/ok.txt");
var_dump(link($target, "../bad/link.txt"));
var_dump(link($target, "../link.txt"));
var_dump(link($target, "../bad/./link.txt"));
var_dump(link($target, "./.././link.txt"));

$link = ($initdir."/test/ok/link.txt");
var_dump(link("../bad/bad.txt", $link));
var_dump(link("../bad", $link));
var_dump(link("../bad/./bad.txt", $link));
var_dump(link("../bad/bad.txt", $link));
var_dump(link("./.././bad", $link));

$target = ($initdir."/test/ok/ok.txt");

var_dump(link($target, $link));
var_dump(unlink($link));
test_open_basedir_after("link");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [link] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: link(): open_basedir restriction in effect. File(%s/test/bad/link.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: link(): open_basedir restriction in effect. File(%s/test/link.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: link(): open_basedir restriction in effect. File(%s/test/bad/link.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: link(): open_basedir restriction in effect. File(%s/test/link.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: link(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: link(): open_basedir restriction in effect. File(%s/test/bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: link(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: link(): open_basedir restriction in effect. File(%s/test/bad/bad.txt) is not within the allowed path(s): (.) in %s on line %d
bool(false)

Warning: link(): open_basedir restriction in effect. File(%s/test/bad) is not within the allowed path(s): (.) in %s on line %d
bool(false)
bool(true)
bool(true)
*** Finished testing open_basedir configuration [link] ***
