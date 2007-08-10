--TEST--
Test tempnam() function: usage variations - invalid/non-existing file
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

/* Passing invalid/non-existing args for $prefix */

echo "*** Testing tempnam() with invalid/non-existing file names ***\n";
$file_path = dirname(__FILE__);

/* An array of names, which will be passed as a file name */ 
$names_arr = array(
  /* Invalid args */ 
  -1,
  TRUE,
  FALSE,
  NULL,
  "",
  " ",
  "/0",
  array(),

  /* Non-existing dirs */
  "/no/such/file/dir", 
  "php"

);

for( $i=1; $i<count($names_arr); $i++ ) {
  echo "-- Iteration $i --\n";
  $file_name = tempnam("$file_path", $names_arr[$i]);

  /* creating the files in existing dir */
  if( file_exists($file_name) ) {
    echo "File name is => ";
    print($file_name);
    echo "\n";

    echo "File permissions are => ";
    printf("%o", fileperms($file_name) );
    echo "\n";
  }
  else
    echo "-- File is not created --\n";

  unlink($file_name);
}

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing tempnam() with invalid/non-existing file names ***
-- Iteration 1 --
File name is => %s
File permissions are => 100600
-- Iteration 2 --
File name is => %s
File permissions are => 100600
-- Iteration 3 --
File name is => %s
File permissions are => 100600
-- Iteration 4 --
File name is => %s
File permissions are => 100600
-- Iteration 5 --
File name is => %s
File permissions are => 100600
-- Iteration 6 --
File name is => %s
File permissions are => 100600
-- Iteration 7 --

Notice: Array to string conversion in %s on line %d
File name is => %s
File permissions are => 100600
-- Iteration 8 --
File name is => %s
File permissions are => 100600
-- Iteration 9 --
File name is => %s
File permissions are => 100600

*** Done ***
