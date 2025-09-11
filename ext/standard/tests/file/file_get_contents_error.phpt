--TEST--
Test file_get_contents() function : error conditions
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php

echo "*** Testing error conditions ***\n";

$file_path = __DIR__;
include($file_path."/file.inc");

echo "\n-- Testing with  Non-existing file --\n";
print( file_get_contents("/no/such/file/or/dir") );

create_files($file_path, 1, "text", 0755, 100, "w", "file_get_contents_error", 1, "byte");
$file_handle = fopen($file_path."/file_put_contents_error.tmp", "w");

echo "\n-- Testing invalid context resource --\n";
try {
    file_get_contents($file_path."/file_get_contents_error1.tmp", FALSE, $file_handle);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "\n-- Testing for invalid negative maxlen values --\n";
try {
    file_get_contents($file_path."/file_get_contents_error1.tmp", FALSE, null, 0, -5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

delete_files($file_path, 1, "file_get_contents_error", 1);
fclose($file_handle);
unlink($file_path."/file_put_contents_error.tmp");

echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
if(file_exists($file_path."/file_put_contents_error.tmp")) {
  unlink($file_path."/file_put_contents_error.tmp");
}
if(file_exists($file_path."/file_put_contents1.tmp")) {
  unlink($file_path."/file_put_contents1.tmp");
}
?>
--EXPECTF--
*** Testing error conditions ***

-- Testing with  Non-existing file --

Warning: file_get_contents(/no/such/file/or/dir): Failed to open stream: No such file or directory in %s on line %d

-- Testing invalid context resource --
TypeError: file_get_contents(): supplied resource is not a valid Stream-Context resource

-- Testing for invalid negative maxlen values --
ValueError: file_get_contents(): Argument #5 ($length) must be greater than or equal to 0

*** Done ***
