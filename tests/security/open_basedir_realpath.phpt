--TEST--
Test open_basedir configuration
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only run on Windows');
}
?>
--INI--
open_basedir=.
--FILE--
<?php
require_once "open_basedir.inc";
test_open_basedir("realpath");
?>
--CLEAN--
<?php
require_once "open_basedir.inc";
delete_directories();
?>
--EXPECTF--
*** Testing open_basedir configuration [realpath] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(%d) "%s\test\bad"
string(%d) "%s\test\bad\bad.txt"
string(%d) "%s\test"
string(%d) "%s\test"
string(%d) "%s"
string(%d) "%s\test\bad"
string(%d) "%s\test\bad\bad.txt"
string(%d) "%s\test\bad\bad.txt"
string(%d) "%s\test"
string(%d) "%s\test\ok"
string(%d) "%s\test\ok\ok.txt"
string(%d) "%s\test\ok\ok.txt"
string(%d) "%s\test\ok\ok.txt"
string(%d) "%s\test\ok\ok.txt"
*** Finished testing open_basedir configuration [realpath] ***
--UEXPECTF--
*** Testing open_basedir configuration [realpath] ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
unicode(%d) "%s\test\bad"
unicode(%d) "%s\test\bad\bad.txt"
unicode(%d) "%s\test"
unicode(%d) "%s\test"
unicode(%d) "%s"
unicode(%d) "%s\test\bad"
unicode(%d) "%s\test\bad\bad.txt"
unicode(%d) "%s\test\bad\bad.txt"
unicode(%d) "%s\test"
unicode(%d) "%s\test\ok"
unicode(%d) "%s\test\ok\ok.txt"
unicode(%d) "%s\test\ok\ok.txt"
unicode(%d) "%s\test\ok\ok.txt"
unicode(%d) "%s\test\ok\ok.txt"
*** Finished testing open_basedir configuration [realpath] ***

