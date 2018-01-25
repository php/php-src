--TEST--
Test file-get_contents() and file_put_contents() functions : error conditions
--FILE--
<?php
/* Prototype: string file_get_contents( string $filename{, bool $use_include_path[,
 *                                      resource $context[, int $offset[, int $maxlen]]]] )
 * Description: Reads entire file into a string
 */

/* Prototype: int file_put_contents( string $filename, mixed $data[, int $flags[, resource $context]] )
 * Description: Write a string to a file
 */

echo "*** Testing error conditions ***\n";

$file_path = dirname(__FILE__);

echo "\n-- Testing with  Non-existing file --\n";
print( file_get_contents("/no/such/file/or/dir") );

echo "\n-- Testing No.of arguments less than expected --\n";
print( file_get_contents() );
print( file_put_contents() );
print( file_put_contents($file_path."/".__FILE__) );

$file_handle = fopen($file_path."/file_put_contents.tmp", "w");
echo "\n-- Testing No.of arguments greater than expected --\n";
print( file_put_contents("abc.tmp", 12345, 1, $file_handle, "extra_argument") );
print( file_get_contents("abc.tmp", false, $file_handle, 1, 2, "extra_argument") );

echo "\n-- Testing for invalid negative maxlen values --";
file_put_contents($file_path."/file_put_contents1.tmp", "Garbage data in the file");
var_dump( file_get_contents($file_path."/file_put_contents1.tmp", FALSE, NULL, 0, -5) );

fclose($file_handle);

echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/file_put_contents.tmp");
unlink($file_path."/file_put_contents1.tmp");

?>
--EXPECTF--
*** Testing error conditions ***

-- Testing with  Non-existing file --

Warning: file_get_contents(/no/such/file/or/dir): failed to open stream: No such file or directory in %s on line %d

-- Testing No.of arguments less than expected --

Warning: file_get_contents() expects at least 1 parameter, 0 given in %s on line %d

Warning: file_put_contents() expects at least 2 parameters, 0 given in %s on line %d

Warning: file_put_contents() expects at least 2 parameters, 1 given in %s on line %d

-- Testing No.of arguments greater than expected --

Warning: file_put_contents() expects at most 4 parameters, 5 given in %s on line %d

Warning: file_get_contents() expects at most 5 parameters, 6 given in %s on line %d

-- Testing for invalid negative maxlen values --
Warning: file_get_contents(): length must be greater than or equal to zero in %s on line %d
bool(false)

*** Done ***
