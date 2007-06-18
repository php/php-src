--TEST--
Test readlink() and realpath functions: basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
?>
--FILE--
<?php
/* Prototype: string readlink ( string $path );
   Description: Returns the target of a symbolic link

   Prototype: string realpath ( string $path );
   Description: Returns canonicalized absolute pathname
*/

/* creating directories, symbolic links and files */
$file_path = dirname(__FILE__);
mkdir("$file_path/readlink_realpath_basic/home/test/", 0777, true);

$file_handle1 = fopen("$file_path/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp", "w");
$file_handle2 = fopen("$file_path/readlink_realpath_basic/home/readlink_realpath_basic.tmp", "w");
$file_handle3 = fopen("$file_path/readlink_realpath_basic/readlink_realpath_basic.tmp", "w");
fclose($file_handle1);
fclose($file_handle2);
fclose($file_handle3);

symlink("$file_path/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp", 
        "$file_path/readlink_realpath_basic/home/test/readlink_realpath_basic_link.tmp");
symlink("$file_path/readlink_realpath_basic/home/readlink_realpath_basic.tmp",
        "$file_path/readlink_realpath_basic/home/readlink_realpath_basic_link.tmp");


echo "*** Testing readlink() and realpath(): with valid and invalid path ***\n";
$linknames = array (
  /* linknames resulting in valid paths */
  "$file_path/readlink_realpath_basic/home/readlink_realpath_basic_link.tmp",
  "$file_path/readlink_realpath_basic/home/test/readlink_realpath_basic_link.tmp",
  "$file_path/readlink_realpath_basic//home/test//../test/./readlink_realpath_basic_link.tmp",
  
  // checking for binary safe
  b"$file_path/readlink_realpath_basic/home/readlink_realpath_basic_link.tmp",

  /* linknames with invalid linkpath */
  "$file_path///readlink_realpath_basic/home//..//././test//readlink_realpath_basic_link.tmp",
  "$file_path/readlink_realpath_basic/home/../home/../test/../readlink_realpath_basic_link.tmp",
  "$file_path/readlink_realpath_basic/..readlink_realpath_basic_link.tmp",
  "$file_path/readlink_realpath_basic/home/test/readlink_realpath_basic_link.tmp/"
);

$counter = 1;
/* loop through $files to read the linkpath of
   the link created from each $file in the above array */
foreach($linknames as $link) {
  echo "\n-- Iteration $counter --\n";
  var_dump( readlink($link) );
  var_dump( realpath($link) );
  $counter++;
}

echo "\n*** Testing realpath() on filenames ***\n";
$filenames = array (
  /* filenames resulting in valid paths */
  "$file_path/readlink_realpath_basic/home/readlink_realpath_basic.tmp",
  "$file_path/readlink_realpath_basic/readlink_realpath_basic.tmp",
  "$file_path/readlink_realpath_basic//home/test//../test/./readlink_realpath_basic.tmp",
  "$file_path/readlink_realpath_basic/home//../././readlink_realpath_basic.tmp",

  // checking for binary safe
  b"$file_path/readlink_realpath_basic/home/readlink_realpath_basic.tmp",

  /* filenames with invalid path */
  "$file_path///readlink_realpath_basic/home//..//././test//readlink_realpath_basic.tmp",
  "$file_path/readlink_realpath_basic/home/../home/../test/../readlink_realpath_basic.tmp",
  "$file_path/readlink_realpath_basic/readlink_realpath_basic.tmp/"
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
$name_prefix = dirname(__FILE__)."/readlink_realpath_basic";
unlink("$name_prefix/home/test/readlink_realpath_basic.tmp");
unlink("$name_prefix/home/readlink_realpath_basic.tmp");
unlink("$name_prefix/readlink_realpath_basic.tmp");
unlink("$name_prefix/home/test/readlink_realpath_basic_link.tmp");
unlink("$name_prefix/home/readlink_realpath_basic_link.tmp");
rmdir("$name_prefix/home/test/");
rmdir("$name_prefix/home/");
rmdir("$name_prefix/");
?>
--EXPECTF--
*** Testing readlink() and realpath(): with valid and invalid path ***

-- Iteration 1 --
string(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"
string(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"

-- Iteration 2 --
string(%d) "%s/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp"
string(%d) "%s/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp"

-- Iteration 3 --
string(%d) "%s/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp"
string(%d) "%s/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp"

-- Iteration 4 --
string(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"
string(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"

-- Iteration 5 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
bool(false)

-- Iteration 6 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
bool(false)

-- Iteration 7 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
bool(false)

-- Iteration 8 --

Warning: readlink(): Not a directory in %s on line %d
bool(false)
bool(false)

*** Testing realpath() on filenames ***

-- Iteration 1 --
string(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"

-- Iteration 2 --
string(%d) "%s/readlink_realpath_basic/readlink_realpath_basic.tmp"

-- Iteration 3 --
string(%d) "%s/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp"

-- Iteration 4 --
string(%d) "%s/readlink_realpath_basic/readlink_realpath_basic.tmp"

-- Iteration 5 --
string(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"

-- Iteration 6 --
bool(false)

-- Iteration 7 --
bool(false)

-- Iteration 8 --
bool(false)
Done
--UEXPECTF--
*** Testing readlink() and realpath(): with valid and invalid path ***

-- Iteration 1 --
unicode(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"
unicode(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"

-- Iteration 2 --
unicode(%d) "%s/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp"
unicode(%d) "%s/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp"

-- Iteration 3 --
unicode(%d) "%s/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp"
unicode(%d) "%s/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp"

-- Iteration 4 --
unicode(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"
unicode(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"

-- Iteration 5 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
bool(false)

-- Iteration 6 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
bool(false)

-- Iteration 7 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
bool(false)

-- Iteration 8 --

Warning: readlink(): Not a directory in %s on line %d
bool(false)
bool(false)

*** Testing realpath() on filenames ***

-- Iteration 1 --
unicode(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"

-- Iteration 2 --
unicode(%d) "%s/readlink_realpath_basic/readlink_realpath_basic.tmp"

-- Iteration 3 --
unicode(%d) "%s/readlink_realpath_basic/home/test/readlink_realpath_basic.tmp"

-- Iteration 4 --
unicode(%d) "%s/readlink_realpath_basic/readlink_realpath_basic.tmp"

-- Iteration 5 --
unicode(%d) "%s/readlink_realpath_basic/home/readlink_realpath_basic.tmp"

-- Iteration 6 --
bool(false)

-- Iteration 7 --
bool(false)

-- Iteration 8 --
bool(false)
Done
