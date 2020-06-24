--TEST--
Test filetype() function: Basic functionality
--FILE--
<?php
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
