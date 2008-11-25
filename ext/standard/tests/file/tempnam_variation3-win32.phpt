--TEST--
Test tempnam() function: usage variations - obscure prefixes
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip run only on Windows");
?>
--FILE--
<?php
/* Prototype:  string tempnam ( string $dir, string $prefix );
   Description: Create file with unique file name.
*/

/* Passing invalid/non-existing args for $prefix */

echo "*** Testing tempnam() with obscure prefixes ***\n";
$file_path = dirname(__FILE__)."/tempnamVar3";
mkdir($file_path);

/* An array of prefixes */ 
$names_arr = array(
  /* Invalid args */ 
  -1,
  TRUE,
  FALSE,
  NULL,
  "",
  " ",
  "\0",
  array(),

  /* prefix with path separator of a non existing directory*/
  "/no/such/file/dir", 
  "php/php"

);

for( $i=0; $i<count($names_arr); $i++ ) {
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
    
    echo "File created in => ";
    $file_dir = dirname($file_name);
    if (realpath($file_dir) == realpath(sys_get_temp_dir()) || realpath($file_dir."\\") == realpath(sys_get_temp_dir())) {
       echo "temp dir\n";
    }
    else if (realpath($file_dir) == realpath($file_path) || realpath($file_dir."\\") == realpath($file_path)) {    
       echo "directory specified\n";
    }
    else {
       echo "unknown location\n";
    }         
  }
  else {
    echo "-- File is not created --\n";
  }

  unlink($file_name);
}

rmdir($file_path);
echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing tempnam() with obscure prefixes ***
-- Iteration 0 --
File name is => %s\%s
File permissions are => 100666
File created in => directory specified
-- Iteration 1 --
File name is => %s\%s
File permissions are => 100666
File created in => directory specified
-- Iteration 2 --
File name is => %s\%s
File permissions are => 100666
File created in => directory specified
-- Iteration 3 --
File name is => %s\%s
File permissions are => 100666
File created in => directory specified
-- Iteration 4 --
File name is => %s\%s
File permissions are => 100666
File created in => directory specified
-- Iteration 5 --
File name is => %s\%s
File permissions are => 100666
File created in => directory specified
-- Iteration 6 --
File name is => %s\%s
File permissions are => 100666
File created in => directory specified
-- Iteration 7 --

Notice: Array to string conversion in %s on line %d
File name is => %s\Ar%s
File permissions are => 100666
File created in => directory specified
-- Iteration 8 --
File name is => %s\di%s
File permissions are => 100666
File created in => directory specified
-- Iteration 9 --
File name is => %s\ph%s
File permissions are => 100666
File created in => directory specified

*** Done ***
