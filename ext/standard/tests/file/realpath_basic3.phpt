--TEST--
Test realpath() with relative paths
--FILE--
<?php
echo "\n*** Testing basic functions of realpath() with files ***\n";

/* creating directories and files */
$file_path = __DIR__;
mkdir("$file_path/realpath_basic3/home/test/", 0777, true);

$file_handle1 = fopen("$file_path/realpath_basic3/home/test/realpath_basic3.tmp", "w");
$file_handle2 = fopen("$file_path/realpath_basic3/home/realpath_basic3.tmp", "w");
$file_handle3 = fopen("$file_path/realpath_basic3/realpath_basic3.tmp", "w");
fclose($file_handle1);
fclose($file_handle2);
fclose($file_handle3);

echo "\n*** Testing realpath() on filenames ***\n";
$filenames = array (
  /* filenames resulting in valid paths */
  "./realpath_basic3/home/realpath_basic3.tmp",
  "./realpath_basic3/realpath_basic3.tmp",
  "./realpath_basic3//home/test//../test/./realpath_basic3.tmp",
  "./realpath_basic3/home//../././realpath_basic3.tmp",

  /* filenames with invalid path */
  // checking for binary safe
  "./realpath_basic3x000/home/realpath_basic3.tmp",

  ".///realpath_basic3/home//..//././test//realpath_basic3.tmp",
  "./realpath_basic3/home/../home/../test/..realpath_basic3.tmp"
);

chdir("$file_path/..");
chdir($file_path);

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
$name_prefix = __DIR__."/realpath_basic3";
unlink("$name_prefix/home/test/realpath_basic3.tmp");
unlink("$name_prefix/home/realpath_basic3.tmp");
unlink("$name_prefix/realpath_basic3.tmp");
rmdir("$name_prefix/home/test/");
rmdir("$name_prefix/home/");
rmdir("$name_prefix/");
?>
--EXPECTF--
*** Testing basic functions of realpath() with files ***

*** Testing realpath() on filenames ***

-- Iteration 1 --
string(%d) "%srealpath_basic3%shome%srealpath_basic3.tmp"

-- Iteration 2 --
string(%d) "%srealpath_basic3%srealpath_basic3.tmp"

-- Iteration 3 --
string(%d) "%srealpath_basic3%shome%stest%srealpath_basic3.tmp"

-- Iteration 4 --
string(%d) "%srealpath_basic3%srealpath_basic3.tmp"

-- Iteration 5 --
bool(false)

-- Iteration 6 --
bool(false)

-- Iteration 7 --
bool(false)
Done
