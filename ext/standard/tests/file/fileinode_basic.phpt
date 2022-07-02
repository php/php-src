--TEST--
Test fileinode() function: Basic functionality
--FILE--
<?php
echo "*** Testing fileinode() with file, directory ***\n";

/* Getting inode of created file */
$file_path = __DIR__;
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
unlink (__DIR__."/inode.tmp");
?>
--EXPECTF--
*** Testing fileinode() with file, directory ***
%i
%i
%i
%i

*** Done ***
