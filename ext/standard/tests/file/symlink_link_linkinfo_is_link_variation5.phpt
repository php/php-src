--TEST--
Test symlink(), linkinfo(), link() and is_link() functions : usage variations - work on deleted link
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    require_once __DIR__ . '/windows_links/common.inc';
    skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
}
?>
--FILE--
<?php
/* Variation 5 : Creating link, deleting it and checking linkinfo(), is_link() on it */

$file_path = __DIR__;

echo "*** Testing linkinfo() and is_link() on deleted link ***\n";
// link name used here
$linkname  = "$file_path/symlink_link_linkinfo_is_link_link_variation5.tmp";

// create temp dir
$dirname = "$file_path/symlink_link_linkinfo_is_link_variation5";
mkdir($dirname);

// filename used here
$filename = "$dirname/symlink_link_linkinfo_is_link_variation5.tmp";
// create the file
$fp = fopen($filename, "w");
$data = "Hello World";
fwrite($fp, $data);
fclose($fp);

var_dump( symlink($filename, $linkname) );  // create link

// delete the link
var_dump( unlink($linkname) );  // delete the link

// clear the cache
clearstatcache();

// try using linkinfo() & is_link() on deleted link; expected: false
$deleted_link = $linkname;
var_dump( linkinfo($deleted_link) );
var_dump( is_link($deleted_link) );

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$dirname = "$file_path/symlink_link_linkinfo_is_link_variation5";
$filename = "$dirname/symlink_link_linkinfo_is_link_variation5.tmp";
unlink($filename);
rmdir($dirname);
?>
--EXPECTF--
*** Testing linkinfo() and is_link() on deleted link ***
bool(true)
bool(true)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)
bool(false)
Done
