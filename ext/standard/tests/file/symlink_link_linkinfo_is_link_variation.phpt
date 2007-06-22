--TEST--
Test symlink(), linkinfo(), link() and is_link() functions : usage variations
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

// creating temp directory which will contain temp file and links created 
$file_path = dirname(__FILE__);
$dirname = "$file_path/symlink_link_linkinfo_is_link/test/home";
mkdir($dirname, 0777, true);

// creating temp file; links are created to this file later on
$filename = "$file_path/symlink_link_linkinfo_is_link/symlink_link_linkinfo_is_link.tmp";
$fp = fopen($filename, "w");
fclose($fp);

/* Variation 1 : Creating links across directories where linkname is stored as an object and array member */
echo "*** Testing symlink(), link(), linkinfo() and is_link() with linknames stored as members in an object ***\n";
class object_temp {
  var $linkname;
  function object_temp($link) {
    $this->linkname = $link;
  }
}

$obj = new object_temp("$dirname/symlink_link_linkinfo_is_link_link.tmp");
/* Testing on soft links */
echo "\n-- Working with soft links --\n";
// creating soft link
var_dump( symlink($filename, $obj->linkname) );
// check if the link exists
var_dump( linkinfo($obj->linkname) );
// check if link is soft link
var_dump( is_link($obj->linkname) );
// delete the link created
unlink($obj->linkname);
// clear the cache
clearstatcache();

/* Testing on hard links */
echo "\n-- Working with hard links --\n";
// creating hard link
var_dump( link($filename, $obj->linkname) ); 
// check if the link exists
var_dump( linkinfo($obj->linkname) );
// check if link is soft link; expected: false as the link is a hardlink
var_dump( is_link($obj->linkname) ); 
// delete the link created
unlink($obj->linkname);
// clear the cache
clearstatcache();

echo "\n*** Testing symlink(), link(), linkinfo() and is_link() with linknames stored as members of an array ***\n";

$link_arr = array("$dirname/symlink_link_linkinfo_is_link_link.tmp");

/* Testing on soft links */
echo "\n-- Working with soft links --\n";
// creating soft link
var_dump( symlink($filename, $link_arr[0]) );
// check if the link exist
var_dump( linkinfo($link_arr[0]) );
// check if link is soft link
var_dump( is_link($link_arr[0]) );
// delete the link created
unlink($link_arr[0]);
// clear the cache
clearstatcache();

/* Testing on hard links */
echo "\n-- Working with hard links --\n";
// creating hard link
var_dump( link($filename, $link_arr[0]) );
// check if the link exist
var_dump( linkinfo($link_arr[0]) );
// check if link is soft link; expected: false as this is a hardlink
var_dump( is_link($link_arr[0]) );
// delete the links created
unlink($link_arr[0]);
// clear the cache
clearstatcache();

/* Variation 2 : Create hard link to non-existent file */
// non-existing filename
$non_existent_file = "$file_path/non_existent_file.tmp";
// non-existing linkname
$non_existent_linkname = "$file_path/non_existent_linkname.tmp";

echo "\n*** Creating a hard link to a non-existent file ***\n";
// creating hard link to non_existent file
var_dump( link($non_existent_file, $non_existent_linkname) ); // expected false
// checking linkinfo() and is_link() on the link; expected: false
var_dump( linkinfo($non_existent_linkname) );
var_dump( is_link($non_existent_linkname) );

/* Variation 3 : Create file and a link to the file
                 Access data of the file through the link 
                 Update the file through link
                 Check size of file and link
*/

echo "\n*** Accessing and updating data of file through soft link ***\n";
// Creating file and inserting data into it
$filename = "$file_path/symlink_link_is_link_linkinfo.tmp";

// create temp file
$file = fopen($filename, "w");

// create soft link to file
$linkname = "$file_path/symlink_is_link_linkinfo_link.tmp";
var_dump( symlink($filename, $linkname) );
// storing size of symlink in a local variable
$link_stat = lstat($linkname);  // lstat of link
$link_size = $link_stat[7];  // size of soft link

// fill data into file
fwrite($file, str_repeat("text", 20) );
fclose($file);

echo "\n-- Access data of the file through the soft link --\n";
$data_from_link = file_get_contents($linkname);  // data read from $filename
var_dump( $data_from_link );

echo "\n-- Check size of soft link and file --\n";
var_dump( filesize($filename) );
var_dump( filesize($linkname) );

// taking lstat of symlink
$stat = lstat($linkname);
// checking that size of symlink remains same
if ($link_size == $stat[7])
  echo "\nSoft link size remains same \n";
else
  echo "\nWarning: Soft link size has changed \n";

echo "\n-- Updating file with data through soft link --\n";
// append link with data
$fp = fopen($linkname, "a");  // open in append mode
fwrite($fp, "Hello World");
fclose($fp);

// now check temp file for data; it should append "Hello World"
$data_from_file = file_get_contents($filename);
var_dump( $data_from_file );

echo "\n-- Check size of soft link and file --\n";
var_dump( filesize($filename) );
var_dump( filesize($linkname) );

// taking lstat of symlink
$stat = lstat($linkname);
// checking that size of symlink remains same
if ($link_size == $stat[7])
  echo "\nSoft link size remains same \n";
else
  echo "\nWarning: Soft link size has changed \n";

echo "\n-- Updating file with data and check data through soft link --\n";
// write to temp file
$file = fopen($filename, "w");
fwrite($file, "Hello World");
fclose($file);

// now check link for data; it should echo "Hello World"
$data_from_link = file_get_contents($linkname);
var_dump( $data_from_link );

echo "\n-- Check size of soft link and file --\n";
var_dump( filesize($filename) );
var_dump( filesize($linkname) );

// taking lstat of symlink
$stat = lstat($linkname);
// checking that size of symlink remains same
if ($link_size == $stat[7])
  echo "\nSoft link size remains same \n";
else
  echo "\nWarning: Soft link size has changed \n";

// delete the link
unlink($linkname);
// delete the temporary file
unlink($filename);

echo "\n*** Accessing and updating data of file through hard link ***\n";
// Creating file and inserting data into it
$filename = "$file_path/symlink_link_is_link_linkinfo.tmp";
// create temp file
$file = fopen($filename, "w");
// fill data into file
fwrite($file, str_repeat("text", 20) );
fclose($file);

echo "\n-- Access data of the file through the hard link --\n";
// create hard link to file
$linkname = "$file_path/symlink_is_link_linkinfo_link.tmp";
var_dump( link($filename, $linkname) );
$data_from_link = file_get_contents($linkname);  // data read from $filename
var_dump( $data_from_link );

echo "\n-- Check size of hard link and file --\n";
if( filesize($filename) == filesize($linkname) ) 
  echo "\nSize of file and hard link are same\n";
else
  echo "\nWarning: Size of file and hard link differ\n";

echo "\n-- Updating file with data through hard link --\n";
// append link with data
$fp = fopen($linkname, "a");  // open in append mode
fwrite($fp, "Hello World");
fclose($fp);

// now check temp file for data; it should append "Hello World"
$data_from_file = file_get_contents($filename);
var_dump( $data_from_file );

echo "\n-- Check size of hard link and file --\n";
if( filesize($filename) == filesize($linkname) ) 
  echo "\nSize of file and hard link are same\n";
else
  echo "\nWarning: Size of file and hard link differ\n";

echo "\n-- Updating file with data and check data through hard link --\n";
// write to temp file
$file = fopen($filename, "w");
fwrite($file, "Hello World");
fclose($file);

// now check link for data; it should echo "Hello World"
$data_from_link = file_get_contents($linkname);
var_dump( $data_from_link );

echo "\n-- Check size of hard link and file --\n";
var_dump( filesize($filename) );
var_dump( filesize($linkname) );
if( filesize($filename) == filesize($linkname) ) 
  echo "\nSize of file and hard link are same\n";
else
  echo "\nWarning: Size of file and hard link differ\n";

// delete the link
unlink($linkname);
// delete the temporary file
unlink($filename);

/* Variation 4 : Creating link, deleting it and checking linkinfo(), is_link() on it */
echo "\n*** Testing linkinfo() and is_link() on non-existent link ***\n";
// link and file names used here
$non_existent_link = "$file_path/non_existent_link.tmp";
$filename = "$file_path/symlink_link_linkinfo_is_link/symlink_link_linkinfo_is_link.tmp";
var_dump( symlink($filename, $non_existent_link) );  // create link
var_dump( unlink($non_existent_link) );  // delete the link
// clear the cache
clearstatcache();
// operating on deleted link; expected: false
var_dump( linkinfo($non_existent_link) );
var_dump( is_link($non_existent_link) );

/* Variation 5 : Change permission of directory and try creating links inside that directory */
echo "\n*** Creating links in a directory without permission to allow the operation ***\n";
// temp file used
$filename = "$file_path/symlink_link_linkinfo_is_link/symlink_link_linkinfo_is_link.tmp";
// remove all permissions from dir
var_dump( chmod($dirname, 0000) );

echo "\n-- Working with soft links --\n";
// expected: false
var_dump( symlink($filename, "$dirname/non_existent_link.tmp") );
var_dump( linkinfo("$dirname/non_existent_link.tmp") );
var_dump( is_link("$dirname/non_existent_link.tmp") );
// clear the cache
clearstatcache();

echo "\n-- Working with hard links --\n";
// expected: false
var_dump( link($filename, "$dirname/non_existent_link.tmp") );
var_dump( linkinfo("$dirname/non_existent_link.tmp") );
var_dump( is_link("$dirname/non_existent_link.tmp") );
// clear the cache
clearstatcache();

chmod($dirname, 0777);  // to enable dir deletion

/* Variation 6 : Create soft/hard link to itself */
// temp file used
$filename = "$file_path/symlink_link_linkinfo_is_link/symlink_link_linkinfo_is_link.tmp";
// link name used
$linkname = "$file_path/symlink_link_linkinfo_is_link/symlink_link_linkinfo_is_link_link.tmp";
// temp dirname used
$dirname = "$file_path/symlink_link_linkinfo_is_link/home/test";

echo "\n*** Create soft link to file and then to itself ***\n";
// create soft link to $filename
var_dump( symlink($filename, $linkname) );
// create another link to $linkname
var_dump( symlink($linkname, $linkname) );
// delete link
unlink($linkname);

echo "\n*** Create soft link to directory and then to itself ***\n";
// create soft link to $dirname
var_dump( symlink($dirname, $linkname) );
// create another link to $dirname
var_dump( symlink($linkname, $linkname) );
// delete link
unlink($linkname);

echo "\n*** Create hard link to file and then to itself ***\n";
// create hard link to $filename
var_dump( link($filename, $linkname) );
// create another link to $linkname
var_dump( link($linkname, $linkname) );
// delete link
unlink($linkname);

/* Variation 7 : Create soft/hard link to different directory */
/* creating link to a file in different dir with the same name as the file */
echo "\n*** Create hard link in different directory with same filename ***\n";
// temp file used
$filename = "$file_path/symlink_link_linkinfo_is_link.tmp";
// temp link name used
mkdir("$file_path/symlink_link_linkinfo_is_link1");
$linkname = "$file_path/symlink_link_linkinfo_is_link1/symlink_link_linkinfo_is_link.tmp";
// create temp file
$fp = fopen($filename, "w");
fclose($fp);

var_dump( link($filename, "$file_path/symlink_link_linkinfo_is_link1/") ); // this fails indicating file exists
// ok, creates "$file_path/symlink_link_linkinfo_is_link1/symlink_link_linkinfo_is_link.tmp" link
var_dump( link($filename, $linkname) );  // this works fine
// delete link
unlink($linkname);
// delete temp file
unlink($filename);
// delete temp dir
rmdir("$file_path/symlink_link_linkinfo_is_link1");

echo "\n*** Create soft link in different directory with same filename ***\n";
// temp file used
$filename = "$file_path/symlink_link_linkinfo_is_link.tmp";
// temp link name used
mkdir("$file_path/symlink_link_linkinfo_is_link1");
$linkname = "$file_path/symlink_link_linkinfo_is_link1/symlink_link_linkinfo_is_link.tmp";
// create temp file
$fp = fopen($filename, "w");
fclose($fp);

var_dump( symlink($filename, "$file_path/symlink_link_linkinfo_is_link1/") ); // this fails indicating file exists
// ok, creates "$file_path/symlink_link_linkinfo_is_link1/symlink_link_linkinfo_is_link.tmp" link
var_dump( symlink($filename, $linkname) );  // this works fine
// delete link
unlink($linkname);
// delete temp file
unlink($filename);
// delete temp dir
rmdir("$file_path/symlink_link_linkinfo_is_link1");

/* Variation 8 : Check lstat of soft/hard link created 
                 Check linkinfo() value with lstat['dev']
*/
// temp file used
$filename = "$file_path/symlink_link_linkinfo_is_link/symlink_link_linkinfo_is_link.tmp";
// soft link name used
$soft_link = "$file_path/symlink_link_linkinfo_is_link/symlink_link_linkinfo_is_link_softlink.tmp";
// hard link name used
$hard_link = "$file_path/symlink_link_linkinfo_is_link/symlink_link_linkinfo_is_link_hardlink.tmp";
// temp dirname used
$dirname = "$file_path/symlink_link_linkinfo_is_link/";

echo "\n*** Checking lstat() on soft link ***\n";
// create soft link
var_dump( symlink($filename, $soft_link) );
// check lstat of link
var_dump( lstat($soft_link) );

// confirming that linkinfo() = lstat['dev'] , this should always match
$linkinfo = linkinfo($soft_link);
$s1 = lstat($soft_link);
if( $s1[0] == $linkinfo )
  echo "\nlinkinfo() value matches lstat['dev']\n";
else
  echo "\nWarning: linkinfo() value doesnt match lstat['dev']\n";
// delete link
unlink($soft_link);

echo "\n*** Checking lstat() on hard link ***\n";
// create hard link
var_dump( link($filename, $hard_link) );
// check lstat of link
var_dump( lstat($hard_link) );

// confirming that linkinfo() = lstat['dev'] , this should always match
$linkinfo = linkinfo($hard_link);
$s1 = lstat($hard_link);
if( $s1[0] == $linkinfo )
  echo "\nlinkinfo() value matches lstat['dev']\n";
else
  echo "\nWarning: linkinfo() value doesnt match lstat['dev']\n";

// delete link
unlink($hard_link);

echo "\n*** Checking lstat() on a soft link to directory ***\n";
// create soft link
var_dump( symlink($dirname, $soft_link) );
// check lstat of link
var_dump( lstat($soft_link) );

// confirming that linkinfo() = lstat['dev'], this should always match
$linkinfo = linkinfo($soft_link);
$s1 = lstat($soft_link);
if( $s1[0] == $linkinfo )
  echo "\nlinkinfo() value matches lstat['dev']\n";
else
  echo "\nWarning: linkinfo() value doesnt match lstat['dev']\n";

// delete link
unlink($soft_link);

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$dirname = "$file_path/symlink_link_linkinfo_is_link";
unlink("$file_path/symlink_link_linkinfo_is_link/symlink_link_linkinfo_is_link.tmp");
rmdir("$dirname/test/home");
rmdir("$dirname/test");
rmdir($dirname);
?>
--EXPECTF--
*** Testing symlink(), link(), linkinfo() and is_link() with linknames stored as members in an object ***

-- Working with soft links --
bool(true)
int(%d)
bool(true)

-- Working with hard links --
bool(true)
int(%d)
bool(false)

*** Testing symlink(), link(), linkinfo() and is_link() with linknames stored as members of an array ***

-- Working with soft links --
bool(true)
int(%d)
bool(true)

-- Working with hard links --
bool(true)
int(%d)
bool(false)

*** Creating a hard link to a non-existent file ***

Warning: link(): No such file or directory in %s on line %d
bool(false)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)
bool(false)

*** Accessing and updating data of file through soft link ***
bool(true)

-- Access data of the file through the soft link --
string(80) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttext"

-- Check size of soft link and file --
int(80)
int(80)

Soft link size remains same 

-- Updating file with data through soft link --
string(91) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttextHello World"

-- Check size of soft link and file --
int(91)
int(91)

Soft link size remains same 

-- Updating file with data and check data through soft link --
string(11) "Hello World"

-- Check size of soft link and file --
int(11)
int(11)

Soft link size remains same 

*** Accessing and updating data of file through hard link ***

-- Access data of the file through the hard link --
bool(true)
string(80) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttext"

-- Check size of hard link and file --

Size of file and hard link are same

-- Updating file with data through hard link --
string(91) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttextHello World"

-- Check size of hard link and file --

Size of file and hard link are same

-- Updating file with data and check data through hard link --
string(11) "Hello World"

-- Check size of hard link and file --
int(11)
int(11)

Size of file and hard link are same

*** Testing linkinfo() and is_link() on non-existent link ***
bool(true)
bool(true)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)
bool(false)

*** Creating links in a directory without permission to allow the operation ***
bool(true)

-- Working with soft links --

Warning: symlink(): Permission denied in %s on line %d
bool(false)

Warning: linkinfo(): Permission denied in %s on line %d
int(-1)
bool(false)

-- Working with hard links --

Warning: link(): Permission denied in %s on line %d
bool(false)

Warning: linkinfo(): Permission denied in %s on line %d
int(-1)
bool(false)

*** Create soft link to file and then to itself ***
bool(true)

Warning: symlink(): File exists in %s on line %d
bool(false)

*** Create soft link to directory and then to itself ***
bool(true)

Warning: symlink(): File exists in %s on line %d
bool(false)

*** Create hard link to file and then to itself ***
bool(true)

Warning: link(): File exists in %s on line %d
bool(false)

*** Create hard link in different directory with same filename ***

Warning: link(): File exists in %s on line %d
bool(false)
bool(true)

*** Create soft link in different directory with same filename ***

Warning: symlink(): File exists in %s on line %d
bool(false)
bool(true)

*** Checking lstat() on soft link ***
bool(true)
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(1)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  ["dev"]=>
  int(%d)
  ["ino"]=>
  int(%d)
  ["mode"]=>
  int(%d)
  ["nlink"]=>
  int(1)
  ["uid"]=>
  int(%d)
  ["gid"]=>
  int(%d)
  ["rdev"]=>
  int(%d)
  ["size"]=>
  int(%d)
  ["atime"]=>
  int(%d)
  ["mtime"]=>
  int(%d)
  ["ctime"]=>
  int(%d)
  ["blksize"]=>
  int(%d)
  ["blocks"]=>
  int(%d)
}

linkinfo() value matches lstat['dev']

*** Checking lstat() on hard link ***
bool(true)
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(2)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  ["dev"]=>
  int(%d)
  ["ino"]=>
  int(%d)
  ["mode"]=>
  int(%d)
  ["nlink"]=>
  int(2)
  ["uid"]=>
  int(%d)
  ["gid"]=>
  int(%d)
  ["rdev"]=>
  int(%d)
  ["size"]=>
  int(%d)
  ["atime"]=>
  int(%d)
  ["mtime"]=>
  int(%d)
  ["ctime"]=>
  int(%d)
  ["blksize"]=>
  int(%d)
  ["blocks"]=>
  int(%d)
}

linkinfo() value matches lstat['dev']

*** Checking lstat() on a soft link to directory ***
bool(true)
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(1)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  ["dev"]=>
  int(%d)
  ["ino"]=>
  int(%d)
  ["mode"]=>
  int(%d)
  ["nlink"]=>
  int(1)
  ["uid"]=>
  int(%d)
  ["gid"]=>
  int(%d)
  ["rdev"]=>
  int(%d)
  ["size"]=>
  int(%d)
  ["atime"]=>
  int(%d)
  ["mtime"]=>
  int(%d)
  ["ctime"]=>
  int(%d)
  ["blksize"]=>
  int(%d)
  ["blocks"]=>
  int(%d)
}

linkinfo() value matches lstat['dev']
Done
--UEXPECTF--
*** Testing symlink(), link(), linkinfo() and is_link() with linknames stored as members in an object ***

-- Working with soft links --
bool(true)
int(%d)
bool(true)

-- Working with hard links --
bool(true)
int(%d)
bool(false)

*** Testing symlink(), link(), linkinfo() and is_link() with linknames stored as members of an array ***

-- Working with soft links --
bool(true)
int(%d)
bool(true)

-- Working with hard links --
bool(true)
int(%d)
bool(false)

*** Creating a hard link to a non-existent file ***

Warning: link(): No such file or directory in %s on line %d
bool(false)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)
bool(false)

*** Accessing and updating data of file through soft link ***
bool(true)

Notice: fwrite(): 80 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

-- Access data of the file through the soft link --
string(80) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttext"

-- Check size of soft link and file --
int(80)
int(80)

Soft link size remains same 

-- Updating file with data through soft link --

Notice: fwrite(): 11 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
string(91) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttextHello World"

-- Check size of soft link and file --
int(91)
int(91)

Soft link size remains same 

-- Updating file with data and check data through soft link --

Notice: fwrite(): 11 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
string(11) "Hello World"

-- Check size of soft link and file --
int(11)
int(11)

Soft link size remains same 

*** Accessing and updating data of file through hard link ***

Notice: fwrite(): 80 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

-- Access data of the file through the hard link --
bool(true)
string(80) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttext"

-- Check size of hard link and file --

Size of file and hard link are same

-- Updating file with data through hard link --

Notice: fwrite(): 11 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
string(91) "texttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttexttextHello World"

-- Check size of hard link and file --

Size of file and hard link are same

-- Updating file with data and check data through hard link --

Notice: fwrite(): 11 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
string(11) "Hello World"

-- Check size of hard link and file --
int(11)
int(11)

Size of file and hard link are same

*** Testing linkinfo() and is_link() on non-existent link ***
bool(true)
bool(true)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)
bool(false)

*** Creating links in a directory without permission to allow the operation ***
bool(true)

-- Working with soft links --

Warning: symlink(): Permission denied in %s on line %d
bool(false)

Warning: linkinfo(): Permission denied in %s on line %d
int(-1)
bool(false)

-- Working with hard links --

Warning: link(): Permission denied in %s on line %d
bool(false)

Warning: linkinfo(): Permission denied in %s on line %d
int(-1)
bool(false)

*** Create soft link to file and then to itself ***
bool(true)

Warning: symlink(): File exists in %s on line %d
bool(false)

*** Create soft link to directory and then to itself ***
bool(true)

Warning: symlink(): File exists in %s on line %d
bool(false)

*** Create hard link to file and then to itself ***
bool(true)

Warning: link(): File exists in %s on line %d
bool(false)

*** Create hard link in different directory with same filename ***

Warning: link(): File exists in %s on line %d
bool(false)
bool(true)

*** Create soft link in different directory with same filename ***

Warning: symlink(): File exists in %s on line %d
bool(false)
bool(true)

*** Checking lstat() on soft link ***
bool(true)
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(1)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  [u"dev"]=>
  int(%d)
  [u"ino"]=>
  int(%d)
  [u"mode"]=>
  int(%d)
  [u"nlink"]=>
  int(1)
  [u"uid"]=>
  int(%d)
  [u"gid"]=>
  int(%d)
  [u"rdev"]=>
  int(%d)
  [u"size"]=>
  int(%d)
  [u"atime"]=>
  int(%d)
  [u"mtime"]=>
  int(%d)
  [u"ctime"]=>
  int(%d)
  [u"blksize"]=>
  int(%d)
  [u"blocks"]=>
  int(%d)
}

linkinfo() value matches lstat['dev']

*** Checking lstat() on hard link ***
bool(true)
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(2)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  [u"dev"]=>
  int(%d)
  [u"ino"]=>
  int(%d)
  [u"mode"]=>
  int(%d)
  [u"nlink"]=>
  int(2)
  [u"uid"]=>
  int(%d)
  [u"gid"]=>
  int(%d)
  [u"rdev"]=>
  int(%d)
  [u"size"]=>
  int(%d)
  [u"atime"]=>
  int(%d)
  [u"mtime"]=>
  int(%d)
  [u"ctime"]=>
  int(%d)
  [u"blksize"]=>
  int(%d)
  [u"blocks"]=>
  int(%d)
}

linkinfo() value matches lstat['dev']

*** Checking lstat() on a soft link to directory ***
bool(true)
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(1)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  [u"dev"]=>
  int(%d)
  [u"ino"]=>
  int(%d)
  [u"mode"]=>
  int(%d)
  [u"nlink"]=>
  int(1)
  [u"uid"]=>
  int(%d)
  [u"gid"]=>
  int(%d)
  [u"rdev"]=>
  int(%d)
  [u"size"]=>
  int(%d)
  [u"atime"]=>
  int(%d)
  [u"mtime"]=>
  int(%d)
  [u"ctime"]=>
  int(%d)
  [u"blksize"]=>
  int(%d)
  [u"blocks"]=>
  int(%d)
}

linkinfo() value matches lstat['dev']
Done
