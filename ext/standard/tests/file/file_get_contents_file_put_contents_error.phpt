--TEST--
Test file-get_contents() and file_put_contents() functions : error conditions
--FILE--
<?php

echo "*** Testing error conditions ***\n";

$file_path = __DIR__;

echo "\n-- Testing with  Non-existing file --\n";
print( file_get_contents("/no/such/file/or/dir") );

echo "\n-- Testing for invalid negative maxlen values --\n";
file_put_contents($file_path."/file_put_contents_error1.tmp", "Garbage data in the file");
try {
    file_get_contents($file_path."/file_put_contents_error1.tmp", FALSE, NULL, 0, -5);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/file_put_contents_error1.tmp");

?>
--EXPECTF--
*** Testing error conditions ***

-- Testing with  Non-existing file --

Warning: file_get_contents(/no/such/file/or/dir): Failed to open stream: No such file or directory in %s on line %d

-- Testing for invalid negative maxlen values --
file_get_contents(): Argument #5 ($length) must be greater than or equal to 0

*** Done ***
