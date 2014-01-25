--TEST--
Test tempnam() function: usage variations - specify a suffix
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Do not run on Windows");
?>
--FILE--
<?php
/* Prototype:  string tempnam ( string $dir, string $prefix, string $suffix );
   Description: Create file with unique file name.
*/

/* Passing args for $prefix and $suffix */

echo "*** Testing tempnam() with suffixes ***\n";
$file_path = dirname(__FILE__)."/tempnamSuffix";
mkdir($file_path);

/* An array of suffixes */ 
$names_arr = array(
  NULL,
  "",
  ".png",
);

for( $i=0; $i<count($names_arr); $i++ ) {
  echo "-- Iteration $i --\n";
  $file_name = tempnam("$file_path", "prefix", $names_arr[$i]);

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
        
    if ($file_dir == sys_get_temp_dir()) {
       echo "temp dir\n";
    }
    else if ($file_dir == $file_path) {
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
*** Testing tempnam() with obscure suffixes ***
-- Iteration 0 --
File name is => %s/prefix%s
File permissions are => 100600
File created in => directory specified
-- Iteration 1 --
File name is => %s/prefix%s
File permissions are => 100600
File created in => directory specified
-- Iteration 2 --
File name is => %s/prefix%s.png
File permissions are => 100600
File created in => directory specified

*** Done ***

