--TEST--
Test chdir() function : usage variations - relative paths
--FILE--
<?php
/* Prototype  : bool chdir(string $directory)
 * Description: Change the current directory 
 * Source code: ext/standard/dir.c
 */

/*
 * Test chdir() with variations of relative paths
 */

echo "*** Testing chdir() : usage variations ***\n";

$base_dir_path = dirname(__FILE__);

$level_one_dir_name = "level_one";
$level_one_dir_path = "$base_dir_path/$level_one_dir_name";

$level_two_dir_name = "level_two";
$level_two_dir_path = "$base_dir_path/$level_one_dir_name/$level_two_dir_name";

// create directories
mkdir($level_one_dir_path);
mkdir($level_two_dir_path);

echo "\n-- \$directory = './level_one': --\n";
var_dump(chdir($base_dir_path));
var_dump(chdir("./$level_one_dir_name"));
var_dump(getcwd());

echo "\n-- \$directory = 'level_one/level_two': --\n";
var_dump(chdir($base_dir_path));
var_dump(chdir("$level_one_dir_name/$level_two_dir_name"));
var_dump(getcwd());

echo "\n-- \$directory = '..': --\n";
var_dump(chdir('..'));
var_dump(getcwd());

echo "\n-- \$directory = 'level_two', '.': --\n";
var_dump(chdir($level_two_dir_path));
var_dump(chdir('.'));
var_dump(getcwd());

echo "\n-- \$directory = '../': --\n";
var_dump(chdir('../'));
var_dump(getcwd());

echo "\n-- \$directory = './': --\n";
var_dump(chdir($level_two_dir_path));
var_dump(chdir('./'));
var_dump(getcwd());

echo "\n-- \$directory = '../../'level_one': --\n";
var_dump(chdir($level_two_dir_path));
var_dump(chdir("../../$level_one_dir_name"));
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
*** Testing chdir() : usage variations ***

-- $directory = './level_one': --
bool(true)
bool(true)
string(%d) "%slevel_one"

-- $directory = 'level_one/level_two': --
bool(true)
bool(true)
string(%d) "%slevel_one%elevel_two"

-- $directory = '..': --
bool(true)
string(%d) "%slevel_one"

-- $directory = 'level_two', '.': --
bool(true)
bool(true)
string(%d) "%slevel_one%elevel_two"

-- $directory = '../': --
bool(true)
string(%d) "%slevel_one"

-- $directory = './': --
bool(true)
bool(true)
string(%d) "%slevel_one%elevel_two"

-- $directory = '../../'level_one': --
bool(true)
bool(true)
string(%d) "%slevel_one"
===DONE===
