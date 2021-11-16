--TEST--
Test symlink(), linkinfo(), link() and is_link() functions: basic functionality - link to files
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    require_once __DIR__ . '/windows_links/common.inc';
    skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
}
?>
--FILE--
<?php
$file_path = __DIR__;

// temp dir created in present working directory
$dirname = "symlink_link_linkinfo_is_link_basic1";
mkdir("$file_path/$dirname");  // creating temp dir

/* Creating soft/hard link to $filename created in temp directory $dirname
   and checking linkinfo() and is_link() on the link created */

echo "*** Testing symlink(), linkinfo(), link() and is_link() : basic functionality ***\n";

// creating file in $dirname, links are created to the this file
$filename = "$file_path/$dirname/symlink_link_linkinfo_is_link_basic1.tmp";
$filename = fopen($filename, "w");
fclose($filename);

// name of the soft link created to $filename
$sym_linkname = "$file_path/$dirname/symlink_link_linkinfo_is_link_softlink_basic1.tmp";

// name of the hard link created to $filename
$linkname = "$file_path/$dirname/symlink_link_linkinfo_is_link_hardlink_basic1.tmp";

// filename stored in array with single and double slash notation in its path
$files = array (
  "$file_path/$dirname/symlink_link_linkinfo_is_link_basic1.tmp",
  "$file_path//$dirname//symlink_link_linkinfo_is_link_basic1.tmp"
);

$counter = 1;
/* create soft/hard link to  the file
   and check linkinfo() and is_link() on the link created */
foreach($files as $file) {
  echo "\n-- Iteration $counter --\n";
  echo "-- Testing on soft links --\n";
  // create soft link
  var_dump( symlink($file, $sym_linkname) );
  // checking information of link with linkinfo()
  $linkinfo = linkinfo($sym_linkname);
  var_dump( is_int($linkinfo) && $linkinfo !== -1 );
  // checking if given file is soft link
  var_dump( is_link($sym_linkname) );
  // clear the cache
  clearstatcache();

  // testing on hard link
  echo "-- Testing on hard links --\n";
  // creating hard link
  var_dump( link($file, $linkname) );
  // checking information of link with linkinfo()
  $linkinfo = linkinfo($sym_linkname);
  var_dump( is_int($linkinfo) && $linkinfo !== -1 );
  // checking if given link is soft link; expected: false
  var_dump( is_link($linkname) );
  // clear the cache
  clearstatcache();

  // deleting the links
  unlink($sym_linkname);
  unlink($linkname);
  $counter++;
}

echo "Done\n";
?>
--CLEAN--
<?php
$dirname = __DIR__."/symlink_link_linkinfo_is_link_basic1";
unlink("$dirname/symlink_link_linkinfo_is_link_basic1.tmp");
rmdir($dirname);
?>
--EXPECT--
*** Testing symlink(), linkinfo(), link() and is_link() : basic functionality ***

-- Iteration 1 --
-- Testing on soft links --
bool(true)
bool(true)
bool(true)
-- Testing on hard links --
bool(true)
bool(true)
bool(false)

-- Iteration 2 --
-- Testing on soft links --
bool(true)
bool(true)
bool(true)
-- Testing on hard links --
bool(true)
bool(true)
bool(false)
Done
