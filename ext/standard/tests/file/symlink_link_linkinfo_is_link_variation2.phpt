--TEST--
Test symlink(), linkinfo(), link() and is_link() functions : usage variations - hardlink to non-existent file
--FILE--
<?php
/* Variation 2 : Create hard link to non-existent file */

$file_path = __DIR__;
// non-existing filename
$non_existent_file = "$file_path/non_existent_file_variation2.tmp";
// non-existing linkname
$non_existent_linkname = "$file_path/non_existent_linkname_variation2.tmp";

echo "*** Creating a hard link to a non-existent file ***\n";
// creating hard link to non_existent file
var_dump( link($non_existent_file, $non_existent_linkname) ); // expected false

// checking linkinfo() and is_link() on the link; expected: false
var_dump( linkinfo($non_existent_linkname) );
var_dump( is_link($non_existent_linkname) );

echo "Done\n";
?>
--EXPECTF--
*** Creating a hard link to a non-existent file ***

Warning: link(): No such file or directory in %s on line %d
bool(false)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)
bool(false)
Done
