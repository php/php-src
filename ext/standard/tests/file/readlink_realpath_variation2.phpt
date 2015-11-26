--TEST--
Test readlink() and realpath() functions: usage variation - linkname/filename stored in array(Bug #42038)
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

echo "*** Testing readlink() and realpath() : usage variations ***\n";
$name_prefix = dirname(__FILE__);
// create temp dir
mkdir("$name_prefix/readlink_realpath_variation2/home/tests/link/", 0777, true);
// create the file
$filename = "$name_prefix/readlink_realpath_variation2/home/tests/link/readlink_realpath_variation2.tmp";
$fp = fopen($filename, "w");
fclose($fp);

echo "\n*** Testing readlink() and realpath() with linkname stored in an array ***\n";
$link_arr = array (
  "$name_prefix////readlink_realpath_variation2/home/tests/link/readlink_realpath_variation2_link.tmp",
  "$name_prefix/./readlink_realpath_variation2/home/../home//tests//..//..//..//home//readlink_realpath_variation2_link.tmp/"
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
  
echo "Done\n";
?>
--CLEAN--
<?php
$name_prefix = dirname(__FILE__)."/readlink_realpath_variation2";
unlink("$name_prefix/home/tests/link/readlink_realpath_variation2.tmp");
rmdir("$name_prefix/home/tests/link/");
rmdir("$name_prefix/home/tests/");
rmdir("$name_prefix/home/");
rmdir("$name_prefix/");
?>
--EXPECTF--
*** Testing readlink() and realpath() : usage variations ***

*** Testing readlink() and realpath() with linkname stored in an array ***

-- Testing readlink() and realpath() with softlink, linkname stored inside an array --
bool(true)
string(%d) "%s/readlink_realpath_variation2/home/tests/link/readlink_realpath_variation2.tmp"
string(%d) "%s/readlink_realpath_variation2/home/tests/link/readlink_realpath_variation2.tmp"

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
bool(false)

Warning: unlink(%s/./readlink_realpath_variation2/home/../home//tests//..//..//..//home//readlink_realpath_variation2_link.tmp/): No such file or directory in %s on line %d

-- Testing readlink() and realpath() with hardlink, linkname stored inside an array --
bool(true)

Warning: readlink(): Invalid argument in %s on line %d
bool(false)
string(%d) "%s/readlink_realpath_variation2/home/tests/link/readlink_realpath_variation2_link.tmp"

Warning: link(): No such file or directory in %s on line %d
bool(false)

Warning: readlink(): No such file or directory in %s on line %d
bool(false)
bool(false)

Warning: unlink(%s/./readlink_realpath_variation2/home/../home//tests//..//..//..//home//readlink_realpath_variation2_link.tmp/): No such file or directory in %s on line %d
Done
