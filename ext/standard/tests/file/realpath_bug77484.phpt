--TEST--
Bug #77484 Zend engine crashes when calling realpath in invalid working dir
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die("skip can't remove CWD on Windows");
}
?>
--FILE--
<?php

var_dump(\getcwd());

\mkdir(__DIR__ . "/foo");
\chdir(__DIR__ . "/foo");
\rmdir(__DIR__ . "/foo");

// Outputs: / (incorrect)
var_dump(\getcwd());

// Outputs: false (correct)
var_dump(\realpath(''));

// Crash
var_dump(\realpath('.'), \realpath('./'));

?>
--EXPECTF--
string(%d) "%s"
bool(false)
bool(false)
string(1) "."
string(1) "."
