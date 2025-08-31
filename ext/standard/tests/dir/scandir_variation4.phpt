--TEST--
Test scandir() function : usage variations - different relative paths
--FILE--
<?php
/*
 * Test scandir() with relative paths as $dir argument
 */

echo "*** Testing scandir() : usage variations ***\n";

// include for create_files/delete_files functions
include (__DIR__ . '/../file/file.inc');

$base_dir_path = __DIR__ . '/scandir_variation4';
@mkdir($base_dir_path);

$level_one_dir_path = "$base_dir_path/level_one";
$level_two_dir_path = "$level_one_dir_path/level_two";

// create directories and files
mkdir($level_one_dir_path);
create_files($level_one_dir_path, 2, 'numeric', 0755, 1, 'w', 'level_one', 1);
mkdir($level_two_dir_path);
create_files($level_two_dir_path, 2, 'numeric', 0755, 1, 'w', 'level_two', 1);

echo "\n-- \$path = './level_one': --\n";
var_dump(chdir($base_dir_path));
var_dump(scandir('./level_one'));

echo "\n-- \$path = 'level_one/level_two': --\n";
var_dump(chdir($base_dir_path));
var_dump(scandir('level_one/level_two'));

echo "\n-- \$path = '..': --\n";
var_dump(chdir($level_two_dir_path));
var_dump(scandir('..'));

echo "\n-- \$path = 'level_two', '.': --\n";
var_dump(chdir($level_two_dir_path));
var_dump(scandir('.'));

echo "\n-- \$path = '../': --\n";
var_dump(chdir($level_two_dir_path));
var_dump(scandir('../'));

echo "\n-- \$path = './': --\n";
var_dump(chdir($level_two_dir_path));
var_dump(scandir('./'));

echo "\n-- \$path = '../../'level_one': --\n";
var_dump(chdir($level_two_dir_path));
var_dump(scandir('../../level_one'));

@delete_files($level_one_dir_path, 2, 'level_one');
@delete_files($level_two_dir_path, 2, 'level_two');
?>
--CLEAN--
<?php
$base_dir_path = __DIR__ . '/scandir_variation4';
rmdir("$base_dir_path/level_one/level_two");
rmdir("$base_dir_path/level_one");
rmdir($base_dir_path);
?>
--EXPECT--
*** Testing scandir() : usage variations ***

-- $path = './level_one': --
bool(true)
array(5) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(14) "level_one1.tmp"
  [3]=>
  string(14) "level_one2.tmp"
  [4]=>
  string(9) "level_two"
}

-- $path = 'level_one/level_two': --
bool(true)
array(4) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(14) "level_two1.tmp"
  [3]=>
  string(14) "level_two2.tmp"
}

-- $path = '..': --
bool(true)
array(5) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(14) "level_one1.tmp"
  [3]=>
  string(14) "level_one2.tmp"
  [4]=>
  string(9) "level_two"
}

-- $path = 'level_two', '.': --
bool(true)
array(4) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(14) "level_two1.tmp"
  [3]=>
  string(14) "level_two2.tmp"
}

-- $path = '../': --
bool(true)
array(5) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(14) "level_one1.tmp"
  [3]=>
  string(14) "level_one2.tmp"
  [4]=>
  string(9) "level_two"
}

-- $path = './': --
bool(true)
array(4) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(14) "level_two1.tmp"
  [3]=>
  string(14) "level_two2.tmp"
}

-- $path = '../../'level_one': --
bool(true)
array(5) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(14) "level_one1.tmp"
  [3]=>
  string(14) "level_one2.tmp"
  [4]=>
  string(9) "level_two"
}
