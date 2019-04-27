--TEST--
Test symlink(), linkinfo(), link() and is_link() functions : usage variations - try link with same name in diff. dir
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

/* Variation 8 : Create soft/hard link to different directory */

/* creating link to a file in different dir with the same name as the file */
echo "\n*** Create hard link in different directory with same filename ***\n";
// temp file used
$file_path = __DIR__;
$filename = "$file_path/symlink_link_linkinfo_is_link_variation8.tmp";
// temp link name used
$dirname = "$file_path/symlink_link_linkinfo_is_link1_variation8";
mkdir($dirname);
$linkname = "symlink_link_linkinfo_is_link_variation8.tmp";
// create temp file
$fp = fopen($filename, "w");
fclose($fp);

var_dump( link($filename, $dirname."/") ); // this fails indicating file exists
// ok, creates "$file_path/symlink_link_linkinfo_is_link1_variation8/symlink_link_linkinfo_is_link_variation8.tmp" link
var_dump( link($filename, $dirname."/".$linkname) );  // this works fine
// delete link
unlink($dirname."/".$linkname);
// delete temp file
unlink($filename);
// delete temp dir
rmdir($dirname);

echo "\n*** Create soft link in different directory with same filename ***\n";
$filename = "$file_path/symlink_link_linkinfo_is_link_variation8.tmp";
// temp link name used
$dirname = "$file_path/symlink_link_linkinfo_is_link1_variation8";
mkdir($dirname);
$linkname = "symlink_link_linkinfo_is_link_variation8.tmp";
// create temp file
$fp = fopen($filename, "w");
fclose($fp);

var_dump( symlink($filename, $dirname."/") ); // this fails indicating file exists
// ok, creates "$file_path/symlink_link_linkinfo_is_link1_variation8/symlink_link_linkinfo_is_link_variation8.tmp" link
var_dump( symlink($filename, $dirname."/".$linkname) );  // this works fine
// delete link
unlink($dirname."/".$linkname);
// delete temp file
unlink($filename);
// delete temp dir
rmdir($dirname);

echo "Done\n";
?>
--EXPECTF--
*** Create hard link in different directory with same filename ***

Warning: link(): File exists in %s on line %d
bool(false)
bool(true)

*** Create soft link in different directory with same filename ***

Warning: symlink(): File exists in %s on line %d
bool(false)
bool(true)
Done
