--TEST--
Test filetype() function: Basic functionality
--FILE--
<?php
/*
Prototype: string filetype ( string $filename );
Description: Returns the type of the file. Possible values are fifo, char,
             dir, block, link, file, and unknown.
*/

echo "*** Testing filetype() with files and dirs ***\n";

print( filetype(__FILE__) )."\n";
print( filetype(".") )."\n";

echo "*** Done ***\n";
?>
--EXPECT--
*** Testing filetype() with files and dirs ***
file
dir
*** Done ***
