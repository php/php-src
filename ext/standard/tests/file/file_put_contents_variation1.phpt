--TEST--
Test file_put_contents() function : variation - test append flag
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing file_put_contents() : variation ***\n";

$filename = "FilePutContentsVar1.tmp";

$data = "The first string to write";
$extra = ", followed by this";

var_dump(file_put_contents($filename, $data));
var_dump(file_put_contents($filename, $extra, FILE_APPEND));
echo filesize($filename)."\n";
readfile($filename);
echo "\n";
clearstatcache();
file_put_contents($filename, $data);
echo filesize($filename)."\n";
readfile($filename);
echo "\n";
unlink($filename);


?>
--EXPECT--
*** Testing file_put_contents() : variation ***
int(25)
int(18)
43
The first string to write, followed by this
25
The first string to write
