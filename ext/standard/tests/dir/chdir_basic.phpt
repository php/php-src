--TEST--
Test chdir() function : basic functionality 
--FILE--
<?php
/* Prototype  : bool chdir(string $directory)
 * Description: Change the current directory 
 * Source code: ext/standard/dir.c
 */

/*
 * Test basic functionality of chdir() with absolute and relative paths
 */

echo "*** Testing chdir() : basic functionality ***\n";
$base_dir_path = dirname(__FILE__);

$level_one_dir_name = "level_one";
$level_one_dir_path = "$base_dir_path/$level_one_dir_name";

$level_two_dir_name = "level_two";
$level_two_dir_path = "$base_dir_path/$level_one_dir_name/$level_two_dir_name";

// create directories
mkdir($level_one_dir_path);
mkdir($level_two_dir_path);

echo "\n-- Testing chdir() with absolute path: --\n";
chdir($base_dir_path);
var_dump(chdir($level_one_dir_path));
var_dump(getcwd());

echo "\n-- Testing chdir() with relative paths: --\n";
var_dump(chdir($level_two_dir_name));
var_dump(getcwd());
?>
===DONE===
--CLEAN--
<?php
$file_path = dirname(__FILE__);
rmdir("$file_path/level_one/level_two");
rmdir("$file_path/level_one");
?>
--EXPECTF--
*** Testing chdir() : basic functionality ***

-- Testing chdir() with absolute path: --
bool(true)
string(%d) "%slevel_one"

-- Testing chdir() with relative paths: --
bool(true)
string(%d) "%slevel_one%elevel_two"
===DONE===
