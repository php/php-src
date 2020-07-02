--TEST--
Test readlink() and realpath functions: basic functionality - diff. path notation for links(Bug #42038)
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    include __DIR__ . '/windows_links/common.inc';
    skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
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
$file_path = __DIR__;
mkdir("$file_path/readlink_realpath_basic1/home/test/", 0777, true);

$file_handle1 = fopen("$file_path/readlink_realpath_basic1/home/test/readlink_realpath_basic1.tmp", "w");
$file_handle2 = fopen("$file_path/readlink_realpath_basic1/home/readlink_realpath_basic1.tmp", "w");
$file_handle3 = fopen("$file_path/readlink_realpath_basic1/readlink_realpath_basic1.tmp", "w");
fclose($file_handle1);
fclose($file_handle2);
fclose($file_handle3);

symlink("$file_path/readlink_realpath_basic1/home/test/readlink_realpath_basic1.tmp",
        "$file_path/readlink_realpath_basic1/home/test/readlink_realpath_basic1_link.tmp");
symlink("$file_path/readlink_realpath_basic1/home/readlink_realpath_basic1.tmp",
        "$file_path/readlink_realpath_basic1/home/readlink_realpath_basic1_link.tmp");


echo "*** Testing readlink() and realpath(): with valid and invalid path ***\n";
$linknames = array (
  /* linknames resulting in valid paths */
  "$file_path/readlink_realpath_basic1/home/readlink_realpath_basic1_link.tmp",
  "$file_path/readlink_realpath_basic1/home/test/readlink_realpath_basic1_link.tmp",
  "$file_path/readlink_realpath_basic1//home/test//../test/./readlink_realpath_basic1_link.tmp",

  /* linknames with invalid linkpath */
  "$file_path///readlink_realpath_basic1/home//..//././test//readlink_realpath_basic1_link.tmp",
  "$file_path/readlink_realpath_basic1/home/../home/../test/../readlink_realpath_basic1_link.tmp",
  "$file_path/readlink_realpath_basic1/..readlink_realpath_basic1_link.tmp",
  "$file_path/readlink_realpath_basic1/home/test/readlink_realpath_basic1_link.tmp/"
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

echo "Done\n";
?>
--CLEAN--
<?php
$name_prefix = __DIR__."/readlink_realpath_basic1";
unlink("$name_prefix/home/test/readlink_realpath_basic1.tmp");
unlink("$name_prefix/home/readlink_realpath_basic1.tmp");
unlink("$name_prefix/readlink_realpath_basic1.tmp");
unlink("$name_prefix/home/test/readlink_realpath_basic1_link.tmp");
unlink("$name_prefix/home/readlink_realpath_basic1_link.tmp");
rmdir("$name_prefix/home/test/");
rmdir("$name_prefix/home/");
rmdir("$name_prefix/");
?>
--EXPECTF--
*** Testing readlink() and realpath(): with valid and invalid path ***

-- Iteration 1 --
string(%d) "%s%ereadlink_realpath_basic1%ehome%ereadlink_realpath_basic1.tmp"
string(%d) "%s%ereadlink_realpath_basic1%ehome%ereadlink_realpath_basic1.tmp"

-- Iteration 2 --
string(%d) "%s%ereadlink_realpath_basic1%ehome%etest%ereadlink_realpath_basic1.tmp"
string(%d) "%s%ereadlink_realpath_basic1%ehome%etest%ereadlink_realpath_basic1.tmp"

-- Iteration 3 --
string(%d) "%s%ereadlink_realpath_basic1%ehome%etest%ereadlink_realpath_basic1.tmp"
string(%d) "%s%ereadlink_realpath_basic1%ehome%etest%ereadlink_realpath_basic1.tmp"

-- Iteration 4 --

Warning: readlink(): %s in %s on line %d
bool(false)
bool(false)

-- Iteration 5 --

Warning: readlink(): %s in %s on line %d
bool(false)
bool(false)

-- Iteration 6 --

Warning: readlink(): %s in %s on line %d
bool(false)
%s

-- Iteration 7 --

Warning: readlink(): %s in %s on line %d
bool(false)
%s
Done
