--TEST--
file_get_contents() function : basic functionality
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php

$file_path = __DIR__;
include($file_path."/file.inc");

echo "*** Testing the basic functionality of the file_get_contents() function ***\n";

echo "-- Testing with simple valid data file --\n";


create_files($file_path, 1, "text", 0755, 100, "w", "file", 1, "byte");
var_dump( file_get_contents($file_path."/file1.tmp") );
delete_files($file_path, 1);

echo "\n-- Testing with empty file --\n";

create_files($file_path, 1, "empty", 0755, 100, "w", "file", 1, "byte");
var_dump( file_get_contents($file_path."/file1.tmp") );

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
include($file_path."/file.inc");
delete_files($file_path, 1);
?>
--EXPECT--
*** Testing the basic functionality of the file_get_contents() function ***
-- Testing with simple valid data file --
string(100) "text text text text text text text text text text text text text text text text text text text text "

-- Testing with empty file --
string(0) ""

*** Done ***
