--TEST--
Test realpath() function: basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only on Windows');
}
?>
--FILE--
<?php
echo "\n*** Testing basic functions of realpath() with files ***\n";

/* creating directories and files */
$file_path = __DIR__;
mkdir("$file_path/realpath_私はガラスを食べられますbasic/home/test/", 0777, true);

$file_handle1 = fopen("$file_path/realpath_私はガラスを食べられますbasic/home/test/realpath_私はガラスを食べられますbasic.tmp", "w");
$file_handle2 = fopen("$file_path/realpath_私はガラスを食べられますbasic/home/realpath_私はガラスを食べられますbasic.tmp", "w");
$file_handle3 = fopen("$file_path/realpath_私はガラスを食べられますbasic/realpath_私はガラスを食べられますbasic.tmp", "w");
fclose($file_handle1);
fclose($file_handle2);
fclose($file_handle3);

echo "\n*** Testing realpath() on filenames ***\n";
$filenames = array (
  /* filenames resulting in valid paths */
  "$file_path/realpath_私はガラスを食べられますbasic/home/realpath_私はガラスを食べられますbasic.tmp",
  "$file_path/realpath_私はガラスを食べられますbasic/realpath_私はガラスを食べられますbasic.tmp/",
  "$file_path/realpath_私はガラスを食べられますbasic//home/test//../test/./realpath_私はガラスを食べられますbasic.tmp",
  "$file_path/realpath_私はガラスを食べられますbasic/home//../././realpath_私はガラスを食べられますbasic.tmp//",

   // checking for binary safe
  "$file_path/realpath_私はガラスを食べられますbasic/home/realpath_私はガラスを食べられますbasic.tmp",

  /* filenames with invalid path */
  "$file_path///realpath_私はガラスを食べられますbasic/home//..//././test//realpath_私はガラスを食べられますbasic.tmp",
  "$file_path/realpath_私はガラスを食べられますbasic/home/../home/../test/../..realpath_私はガラスを食べられますbasic.tmp"
);

$counter = 1;
/* loop through $files to read the filepath of $file in the above array */
foreach($filenames as $file) {
  echo "\n-- Iteration $counter --\n";
  var_dump( realpath($file) );
  $counter++;
}

echo "Done\n";
?>
--CLEAN--
<?php
$name_prefix = __DIR__."/realpath_私はガラスを食べられますbasic";
unlink("$name_prefix/home/test/realpath_私はガラスを食べられますbasic.tmp");
unlink("$name_prefix/home/realpath_私はガラスを食べられますbasic.tmp");
unlink("$name_prefix/realpath_私はガラスを食べられますbasic.tmp");
rmdir("$name_prefix/home/test/");
rmdir("$name_prefix/home/");
rmdir("$name_prefix/");
?>
--EXPECTF--
*** Testing basic functions of realpath() with files ***

*** Testing realpath() on filenames ***

-- Iteration 1 --
string(%d) "%s\realpath_私はガラスを食べられますbasic\home\realpath_私はガラスを食べられますbasic.tmp"

-- Iteration 2 --
bool(false)

-- Iteration 3 --
string(%d) "%s\realpath_私はガラスを食べられますbasic\home\test\realpath_私はガラスを食べられますbasic.tmp"

-- Iteration 4 --
bool(false)

-- Iteration 5 --
string(%d) "%s\realpath_私はガラスを食べられますbasic\home\realpath_私はガラスを食べられますbasic.tmp"

-- Iteration 6 --
bool(false)

-- Iteration 7 --
bool(false)
Done
