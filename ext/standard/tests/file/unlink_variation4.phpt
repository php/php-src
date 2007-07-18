--TEST--
Test unlink() function : usage variations - unlink deleted file
--FILE--
<?php
/* Prototype : bool unlink ( string $filename [, resource $context] );
   Description : Deletes filename
*/

$file_path = dirname(__FILE__);

// temp file used
$filename = "$file_path/unlink_variation4.tmp";

/* Try deleting a file which is already deleted */
 
echo "*** Testing unlink() on deleted file ***\n";
// create temp file
$fp = fopen($filename, "w");
fclose($fp);

// delete temp file
var_dump( unlink($filename) );  // expected: true
var_dump( file_exists($filename) );  // confirm file deleted

// delete deleted file
var_dump( unlink($filename) );  // expected: false

echo "Done\n";
?>
--EXPECTF--
*** Testing unlink() on deleted file ***
bool(true)
bool(false)

Warning: unlink(%s/unlink_variation4.tmp): %s in %s on line %d
bool(false)
Done
