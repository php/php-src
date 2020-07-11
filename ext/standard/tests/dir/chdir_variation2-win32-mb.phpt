--TEST--
Test chdir() function : usage variations - relative paths
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/*
 * Test chdir() with variations of relative paths
 */

echo "*** Testing chdir() : usage variations ***\n";

$base_dir_path = __DIR__ . '/chdir_variation2-win32-mb';
@mkdir($base_dir_path);

$level_one_dir_name = "私はガラスを食べられますlevel_one";
$level_one_dir_path = "$base_dir_path/$level_one_dir_name";

$level_two_dir_name = "私はガラスを食べられますlevel_two";
$level_two_dir_path = "$base_dir_path/$level_one_dir_name/$level_two_dir_name";

// create directories
mkdir($level_one_dir_path);
mkdir($level_two_dir_path);

echo "\n-- \$directory = './私はガラスを食べられますlevel_one': --\n";
var_dump(chdir($base_dir_path));
var_dump(chdir("./$level_one_dir_name"));
var_dump(getcwd());

echo "\n-- \$directory = '私はガラスを食べられますlevel_one/私はガラスを食べられますlevel_two': --\n";
var_dump(chdir($base_dir_path));
var_dump(chdir("$level_one_dir_name/$level_two_dir_name"));
var_dump(getcwd());

echo "\n-- \$directory = '..': --\n";
var_dump(chdir('..'));
var_dump(getcwd());

echo "\n-- \$directory = '私はガラスを食べられますlevel_two', '.': --\n";
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

echo "\n-- \$directory = '../../'私はガラスを食べられますlevel_one': --\n";
var_dump(chdir($level_two_dir_path));
var_dump(chdir("../../$level_one_dir_name"));
var_dump(getcwd());
?>
--CLEAN--
<?php
$base_dir_path = __DIR__ . '/chdir_variation2-win32-mb';
chdir(__DIR__); /* not that PWD is accidentialy one of the dirs to be deleted. */
rmdir("$base_dir_path/私はガラスを食べられますlevel_one/私はガラスを食べられますlevel_two");
rmdir("$base_dir_path/私はガラスを食べられますlevel_one");
rmdir($base_dir_path);
?>
--EXPECTF--
*** Testing chdir() : usage variations ***

-- $directory = './私はガラスを食べられますlevel_one': --
bool(true)
bool(true)
string(%d) "%slevel_one"

-- $directory = '私はガラスを食べられますlevel_one/私はガラスを食べられますlevel_two': --
bool(true)
bool(true)
string(%d) "%s私はガラスを食べられますlevel_one%e私はガラスを食べられますlevel_two"

-- $directory = '..': --
bool(true)
string(%d) "%s私はガラスを食べられますlevel_one"

-- $directory = '私はガラスを食べられますlevel_two', '.': --
bool(true)
bool(true)
string(%d) "%s私はガラスを食べられますlevel_one%e私はガラスを食べられますlevel_two"

-- $directory = '../': --
bool(true)
string(%d) "%slevel_one"

-- $directory = './': --
bool(true)
bool(true)
string(%d) "%s私はガラスを食べられますlevel_one%e私はガラスを食べられますlevel_two"

-- $directory = '../../'私はガラスを食べられますlevel_one': --
bool(true)
bool(true)
string(%d) "%s私はガラスを食べられますlevel_one"
