--TEST--
Test symlink(), linkinfo(), link() and is_link() functions: basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
?>
--FILE--
<?php
/* Prototype: bool symlink ( string $target, string $link );
   Description: creates a symbolic link to the existing target with the specified name link

   Prototype: bool is_link ( string $filename );
   Description: Tells whether the given file is a symbolic link.

   Prototype: bool link ( string $target, string $link );
   Description: Create a hard link

   Prototype: int linkinfo ( string $path );
   Description: Gets information about a link
*/

$file_path = dirname(__FILE__);

// temp dir created in present working directory
$dirname = "symlink_linkinfo_link_is_link";
mkdir("$file_path/$dirname");  // creating temp dir

/* Creating soft/hard link to $filename created in temp directory $dirname
   and checking linkinfo() and is_link() on the link created */

echo "*** Testing symlink(), linkinfo(), link() and is_link() : basic functionality ***\n";

// creating file in $dirname, links are created to the this file
$filename = "$file_path/$dirname/symlink_is_link_linkinfo_link.tmp";
$filename = fopen($filename, "w");
fclose($filename);

// name of the soft link created to $filename
$sym_linkname = "$file_path/$dirname/symlink_linkinfo_link_is_link_softlink.tmp";

// name of the hard link created to $filename
$linkname = "$file_path/$dirname/symlink_linkinfo_link_is_link_hardlink.tmp";

// filename stored in array with single and double slash notation in its path
$files = array (
  "$file_path/$dirname/symlink_is_link_linkinfo_link.tmp",
  "$file_path//$dirname//symlink_is_link_linkinfo_link.tmp"
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
  var_dump( linkinfo($sym_linkname) );
  // checking if given file is soft link
  var_dump( is_link($sym_linkname) );
  // clear the cache
  clearstatcache();

  // testing on hard link
  echo "-- Testing on hard links --\n";
  // creating hard link
  var_dump( link($file, $linkname) );
  // checking information of link with linkinfo()
  var_dump( linkinfo($linkname) );
  // checking if given link is soft link; expected: false
  var_dump( is_link($linkname) );
  // clear the cache
  clearstatcache();

  // deleting the links
  unlink($sym_linkname);
  unlink($linkname);
  $counter++;
}

/* Creating soft/hard link to the temporary dir $dirname and checking
   linkinfo() and is_link() on the link created to $dirname */

$dirname = "symlink_linkinfo_link_is_link";
echo "\n*** Testing symlink(), linkinfo(), link() and is_link() on directory ***\n";

// name of the soft link created to $dirname
$sym_linkname = "$file_path/$dirname/symlink_linkinfo_link_is_link_softlink.tmp";

// name of the hard link created to $dirname
$linkname = "$file_path/$dirname/symlink_linkinfo_link_is_link_hardlink.tmp";

// testing on soft link
echo "\n-- Testing on soft links --\n";
// creating soft link to $dirname
var_dump( symlink("$file_path/$dirname", $sym_linkname) ); // this works, expected true
// gets information about soft link created to directory; expected: true
var_dump( linkinfo($sym_linkname) );
// checks if link created is soft link; expected: true
var_dump( is_link($sym_linkname) );
// clear the cache
clearstatcache();

// testing on hard link
echo "\n-- Testing on hard links --\n";
// creating hard link to $dirname; expected: false
var_dump( link("$file_path/$dirname", $linkname) ); // this doesn't work, expected false
var_dump( linkinfo($linkname) ); // link doesn't exists as not created, expected false
var_dump( is_link($linkname) ); // link doesn't exists as not created, expected false
// clear the cache
clearstatcache();

// deleting the links
unlink($sym_linkname);

echo "Done\n";
?>
--CLEAN--
<?php
$dirname = dirname(__FILE__)."/symlink_linkinfo_link_is_link";
unlink("$dirname/symlink_is_link_linkinfo_link.tmp");
rmdir($dirname);
?>
--EXPECTF--
*** Testing symlink(), linkinfo(), link() and is_link() : basic functionality ***

-- Iteration 1 --
-- Testing on soft links --
bool(true)
int(%d)
bool(true)
-- Testing on hard links --
bool(true)
int(%d)
bool(false)

-- Iteration 2 --
-- Testing on soft links --
bool(true)
int(%d)
bool(true)
-- Testing on hard links --
bool(true)
int(%d)
bool(false)

*** Testing symlink(), linkinfo(), link() and is_link() on directory ***

-- Testing on soft links --
bool(true)
int(%d)
bool(true)

-- Testing on hard links --

Warning: link(): Operation not permitted in %s on line %d
bool(false)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)
bool(false)
Done
--UEXPECTF--
*** Testing symlink(), linkinfo(), link() and is_link() : basic functionality ***

-- Iteration 1 --
-- Testing on soft links --
bool(true)
int(%d)
bool(true)
-- Testing on hard links --
bool(true)
int(%d)
bool(false)

-- Iteration 2 --
-- Testing on soft links --
bool(true)
int(%d)
bool(true)
-- Testing on hard links --
bool(true)
int(%d)
bool(false)

*** Testing symlink(), linkinfo(), link() and is_link() on directory ***

-- Testing on soft links --
bool(true)
int(%d)
bool(true)

-- Testing on hard links --

Warning: link(): Operation not permitted in %s on line %d
bool(false)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)
bool(false)
Done
