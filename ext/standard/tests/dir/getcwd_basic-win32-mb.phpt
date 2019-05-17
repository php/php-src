--TEST--
Test getcwd() function : basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/* Prototype  : mixed getcwd(void)
 * Description: Gets the current directory
 * Source code: ext/standard/dir.c
 */

/*
 * Test basic functionality of getcwd()
 */

echo "*** Testing getcwd() : basic functionality ***\n";

//create temporary directory for test, removed in CLEAN section
$directory = __DIR__ . "/私はガラスを食べられますgetcwd_basic";
mkdir($directory);

var_dump(getcwd());
chdir($directory);
var_dump(getcwd());
?>
===DONE===
--CLEAN--
<?php
$directory = __DIR__ . "/私はガラスを食べられますgetcwd_basic";
rmdir($directory);
?>
--EXPECTF--
*** Testing getcwd() : basic functionality ***
string(%d) "%s"
string(%d) "%s%e私はガラスを食べられますgetcwd_basic"
===DONE===
