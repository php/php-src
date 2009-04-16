--TEST--
Test file_get_contents() function : error conditions
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype: string file_get_contents( string $filename{, bool $use_include_path[,
 *                                      resource $context[, int $offset[, int $maxlen]]]] ) 
 * Description: Reads entire file into a string
 */

echo "*** Testing error conditions ***\n";

$file_path = dirname(__FILE__);
include($file_path."/file.inc");

echo "\n-- Testing with  Non-existing file --\n";
print( file_get_contents("/no/such/file/or/dir") );

echo "\n-- Testing No.of arguments less than expected --\n";
print( file_get_contents() );

echo "\n-- Testing No.of arguments greater than expected --\n";

create_files($file_path, 1, "text", 0755, 100, "w", "file", 1, "byte");
$file_handle = fopen($file_path."/file_put_contents_error.tmp", "w");
print( file_get_contents($file_path."/file1.tmp", false, $file_handle, 1, 2, "extra_argument") );

echo "\n-- Testing for invalid negative maxlen values --";
var_dump( file_get_contents($file_path."/file1.tmp", FALSE, $file_handle, 0, -5) );
   
delete_files($file_path, 1);
fclose($file_handle);
unlink($file_path."/file_put_contents_error.tmp");

echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
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

Warning: file_get_contents(/no/such/file/or/dir): failed to open stream: No such file or directory in %s on line %d

-- Testing No.of arguments less than expected --

Warning: file_get_contents() expects at least 1 parameter, 0 given in %s on line %d

-- Testing No.of arguments greater than expected --

Warning: file_get_contents() expects at most 5 parameters, 6 given in %s on line %d

-- Testing for invalid negative maxlen values --
Warning: file_get_contents(): length must be greater than or equal to zero in %s on line %d
bool(false)

*** Done ***
