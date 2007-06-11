--TEST--
Test readlink() and realpath() functions: usage variation
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

require dirname(__FILE__).'/file.inc';

echo "*** Testing readlink() and realpath() : usage variations ***\n";
$name_prefix = dirname(__FILE__);
$filename = "$name_prefix/readlink_realpath_variation/home/tests/link/readlink_realpath_variation.tmp";
mkdir("$name_prefix/readlink_realpath_variation/home/tests/link/", 0777, true);

echo "\n*** Testing readlink() and realpath() with linkname stored inside a object ***\n";
// create a temp file
$file_handle = fopen($filename, "w");
fclose($file_handle);

// creating object with members as linkname
class object_temp {
  public $linkname;
  function object_temp($link) {
    $this->linkname = $link;
  }
}
$obj1 = new object_temp("$name_prefix/readlink_realpath_variation/../././readlink_realpath_variation/home/readlink_realpath_variation_link.tmp");
$obj2 = new object_temp("$name_prefix/readlink_realpath_variation/home/../..///readlink_realpath_variation_link.tmp");

echo "\n-- Testing readlink() and realpath() with softlink, linkname stored inside an object --\n";
// creating the links 
var_dump( symlink($filename, $obj1->linkname) );  
var_dump( readlink($obj1->linkname) ); 
var_dump( realpath($obj1->linkname) );
var_dump( symlink($filename, $obj2->linkname) ); 
var_dump( readlink($obj2->linkname) );
var_dump( realpath($obj2->linkname) );

// deleting the link
unlink($obj1->linkname);
unlink($obj2->linkname);  

echo "\n-- Testing readlink() and realpath() with hardlink, linkname stored inside an object --\n";
// creating hard links
var_dump( link($filename, $obj1->linkname) );  
var_dump( readlink($obj1->linkname) );   // invalid because readlink doesn't work with hardlink
var_dump( realpath($obj1->linkname) );
var_dump( link($filename, $obj2->linkname) );  
var_dump( readlink($obj2->linkname) );   // invalid because readlink doesn't work with hardlink
var_dump( realpath($obj2->linkname) );

// delete the links 
unlink($obj1->linkname);
unlink($obj2->linkname);  

echo "\n*** Testing readlink() and realpath() with linkname stored in an array ***\n";
$link_arr = array (
  "$name_prefix////readlink_realpath_variation/home/tests/link/readlink_realpath_variation_link.tmp",
  "$name_prefix/./readlink_realpath_variation/home/../home//tests//..//..//..//home//readlink_realpath_variation_link.tmp/"
);

echo "\n-- Testing readlink() and realpath() with softlink, linkname stored inside an array --\n";
// creating the links 
var_dump( symlink($filename, $link_arr[0]) );  
var_dump( readlink($link_arr[0]) ); 
var_dump( realpath($link_arr[0]) ); 
var_dump( symlink($filename, $link_arr[1]) ); 
var_dump( readlink($link_arr[1]) );
var_dump( realpath($link_arr[1]) );

// deleting the link
unlink($link_arr[0]);
unlink($link_arr[1]);  

echo "\n-- Testing readlink() and realpath() with hardlink, linkname stored inside an array --\n";
// creating hard links
var_dump( link($filename, $link_arr[0]) );  
var_dump( readlink($link_arr[0]) );   // invalid because readlink doesn't work with hardlink
var_dump( realpath($link_arr[0]) );
var_dump( link($filename, $link_arr[1]) );  
var_dump( readlink($link_arr[1]) );   // invalid because readlink doesn't work with hardlink
var_dump( realpath($link_arr[1]) );

// delete the links 
unlink($link_arr[0]);
unlink($link_arr[1]);  
  
echo "\n*** Testing readlink() and realpath() with linkname as empty string, NULL and single space ***\n";
$link_string = array (
  /* linkname as spaces */
  " ",
  ' ',

  /* empty linkname */
  "",
  '',
  NULL,
  null
 );
for($loop_counter = 0; $loop_counter < count($link_string); $loop_counter++) {
  echo "-- Iteration";
  echo $loop_counter + 1;
  echo " --\n";

  var_dump( readlink($link_string[$loop_counter]) ); 
  var_dump( realpath($link_string[$loop_counter]) ); 
}

echo "Done\n";
?>
--CLEAN--
<?php
$name_prefix = dirname(__FILE__)."/readlink_realpath_variation";
unlink("$name_prefix/home/tests/link/readlink_realpath_variation.tmp");
rmdir("$name_prefix/home/tests/link/");
rmdir("$name_prefix/home/tests/");
rmdir("$name_prefix/home/");
rmdir("$name_prefix/");
?>
--EXPECTF--
*** Testing readlink() and realpath() : usage variations ***

*** Testing readlink() and realpath() with linkname stored inside a object ***

-- Testing readlink() and realpath() with softlink, linkname stored inside an object --
bool(true)
string(%d) "%s/readlink_realpath_variation/home/tests/link/readlink_realpath_variation.tmp"
string(%d) "%s/readlink_realpath_variation/home/readlink_realpath_variation_link.tmp"
bool(true)
string(%d) "%s/readlink_realpath_variation/home/tests/link/readlink_realpath_variation.tmp"
string(%d) "%s/readlink_realpath_variation_link.tmp"

-- Testing readlink() and realpath() with hardlink, linkname stored inside an object --
bool(true)

Warning: readlink(): Invalid argument in %s on line %d
bool(false)
string(%d) "%s/readlink_realpath_variation/home/readlink_realpath_variation_link.tmp"
bool(true)

Warning: readlink(): Invalid argument in %s on line %d
bool(false)
string(%d) "%s/readlink_realpath_variation_link.tmp"

*** Testing readlink() and realpath() with linkname stored in an array ***

-- Testing readlink() and realpath() with softlink, linkname stored inside an array --
bool(true)
string(%d) "%s/readlink_realpath_variation/home/tests/link/readlink_realpath_variation.tmp"
string(%d) "%s/readlink_realpath_variation/home/tests/link/readlink_realpath_variation_link.tmp"

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%s/home/readlink_realpath_variation_link.tmp"

Warning: unlink(%s/./readlink_realpath_variation/home/../home//tests//..//..//..//home//readlink_realpath_variation_link.tmp/): No such file or directory in %s on line %d

-- Testing readlink() and realpath() with hardlink, linkname stored inside an array --
bool(true)

Warning: readlink(): Invalid argument in %s on line %d
bool(false)
string(%d) "%s/readlink_realpath_variation/home/tests/link/readlink_realpath_variation_link.tmp"

Warning: link(): No such file or directory in %s on line %d
bool(false)

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%s/home/readlink_realpath_variation_link.tmp"

Warning: unlink(%s/./readlink_realpath_variation/home/../home//tests//..//..//..//home//readlink_realpath_variation_link.tmp/): No such file or directory in %s on line %d

*** Testing readlink() and realpath() with linkname as empty string, NULL and single space ***
-- Iteration1 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
bool(false)
-- Iteration2 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
bool(false)
-- Iteration3 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%s"
-- Iteration4 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%s"
-- Iteration5 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%s"
-- Iteration6 --

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
string(%d) "%s"
Done
