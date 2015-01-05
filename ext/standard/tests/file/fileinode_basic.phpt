--TEST--
Test fileinode() function: Basic functionality
--FILE--
<?php
/* 
Prototype: int fileinode ( string $filename );
Description: Returns the inode number of the file, or FALSE in case of an error.
*/

echo "*** Testing fileinode() with file, directory ***\n";

/* Getting inode of created file */
$file_path = dirname(__FILE__);
fopen("$file_path/inode.tmp", "w");
print( fileinode("$file_path/inode.tmp") )."\n";

/* Getting inode of current file */
print( fileinode(__FILE__) )."\n";

/* Getting inode of directories */
print( fileinode(".") )."\n";
print( fileinode("./..") )."\n";

echo "\n*** Done ***";
--CLEAN--
<?php
unlink (dirname(__FILE__)."/inode.tmp");
?>
--EXPECTF--
*** Testing fileinode() with file, directory ***
%d
%d
%d
%d

*** Done ***
