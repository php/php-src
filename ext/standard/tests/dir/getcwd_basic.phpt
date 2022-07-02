--TEST--
Test getcwd() function : basic functionality
--FILE--
<?php
/*
 * Test basic functionality of getcwd()
 */

echo "*** Testing getcwd() : basic functionality ***\n";

//create temporary directory for test, removed in CLEAN section
$directory = __DIR__ . "/getcwd_basic";
mkdir($directory);

var_dump(getcwd());
chdir($directory);
var_dump(getcwd());
?>
--CLEAN--
<?php
$directory = __DIR__ . "/getcwd_basic";
rmdir($directory);
?>
--EXPECTF--
*** Testing getcwd() : basic functionality ***
string(%d) "%s"
string(%d) "%s%egetcwd_basic"
