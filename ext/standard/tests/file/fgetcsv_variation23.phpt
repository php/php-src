--TEST--
Test fgetcsv() : usage variations - empty file
--FILE--
<?php
/* 
 Prototype: array fgetcsv ( resource $handle [, int $length [, string $delimiter [, string $enclosure]]] );
 Description: Gets line from file pointer and parse for CSV fields
*/

/* Testing fgetcsv() to read from an empty file */

echo "*** Testing fgetcsv() : reading from file which is having zero content ***\n";

// try reading from file which is having zero content
// create the file and then open in read mode and try reading 
$filename = dirname(__FILE__) . '/fgetcsv_variation23.tmp';
$fp = fopen ($filename, "w");
fclose($fp);
$fp = fopen ($filename, "r");
if (!$fp) {
  echo "Error: failed to create file $filename!\n";
  exit();
}
var_dump( fgetcsv($fp) );
var_dump( ftell($fp) );
var_dump( fgetcsv($fp, 1024) );
var_dump( ftell($fp) );
var_dump( fgetcsv($fp, 1024, "+" ) );
var_dump( ftell($fp) );
var_dump( fgetcsv($fp, 1024, "+", "%") );
var_dump( ftell($fp) );

// close and delete the file
fclose($fp);
unlink($filename);
echo "Done\n";
?>
--EXPECT--
*** Testing fgetcsv() : reading from file which is having zero content ***
bool(false)
int(0)
bool(false)
int(0)
bool(false)
int(0)
bool(false)
int(0)
Done
