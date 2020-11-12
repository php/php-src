--TEST--
Bug #77484 Zend engine crashes when calling realpath in invalid working dir
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die("skip can't remove CWD on Windows");
}
if (PHP_ZTS) {
    /* TODO eliminate difference in TS build. */
    die("skip Not for ZTS");
}
if (getenv('SKIP_REPEAT')) {
    /* The cwd is persistent across repeats */
    die("skip Not repeatable");
}
?>
--FILE--
<?php

$old_cwd = getcwd();

mkdir(__DIR__ . "/foo");
chdir(__DIR__ . "/foo");
rmdir(__DIR__ . "/foo");

// Outputs: / (incorrect)
$new_cwd = getcwd();

// Outputs: false (correct)
$rp0 = realpath('');

// Crash
$rp1 = realpath('.');
$rp2 = realpath('./');

var_dump($old_cwd, $new_cwd, $rp0, $rp1, $rp2);
?>
--EXPECTF--
string(%d) "%s"
bool(false)
bool(false)
string(1) "."
string(1) "."
