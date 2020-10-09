--TEST--
Test opendir() function : error conditions - Non-existent directory
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--FILE--
<?php
/*
 * Pass a non-existent directory as $path argument to opendir() to test behaviour
 */

echo "*** Testing opendir() : error conditions ***\n";

echo "\n-- Pass a non-existent absolute path: --\n";
$path = __DIR__ . "/idonotexist";
var_dump(opendir($path));

echo "\n-- Pass a non-existent relative path: --\n";
chdir(__DIR__);
var_dump(opendir('idonotexist'));
?>
--EXPECTF--
*** Testing opendir() : error conditions ***

-- Pass a non-existent absolute path: --

Warning: opendir(%s/idonotexist): Failed to open directory: %s in %s on line %d
bool(false)

-- Pass a non-existent relative path: --

Warning: opendir(idonotexist): Failed to open directory: %s in %s on line %d
bool(false)
