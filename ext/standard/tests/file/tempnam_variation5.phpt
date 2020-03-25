--TEST--
Test tempnam() function: usage variations - existing file
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Do not run on Windows");
?>
--FILE--
<?php
/* Prototype:  string tempnam ( string $dir, string $prefix );
   Description: Create file with unique file name.
*/

/* Passing an existing file as $prefix for tempnam() fn */

$file_path = __DIR__;

echo "*** Test tempnam() function: by passing an existing filename as prefix ***\n";
$dir_name = $file_path."/tempnam_variation5";
mkdir($dir_name);
$h = fopen($dir_name."/tempnam_variation5.tmp", "w");

for($i=1; $i<=3; $i++) {
  echo "-- Iteration $i --\n";
  $created_file = tempnam("$dir_name", "tempnam_variation5.tmp");

  if( file_exists($created_file) ) {
    echo "File name is => ";
    print($created_file);
    echo "\n";
  }
  else
    print("File is not created");

  unlink($created_file);
}
fclose($h);
unlink($dir_name."/tempnam_variation5.tmp");
rmdir($dir_name);

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Test tempnam() function: by passing an existing filename as prefix ***
-- Iteration 1 --
File name is => %stempnam_variation5%etempnam_variation5.tmp%s
-- Iteration 2 --
File name is => %stempnam_variation5%etempnam_variation5.tmp%s
-- Iteration 3 --
File name is => %stempnam_variation5%etempnam_variation5.tmp%s

*** Done ***
