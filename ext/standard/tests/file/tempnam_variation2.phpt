--TEST--
Test tempnam() function: usage variations - relative paths
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

/* Creating unique files in various dirs by passing relative paths to $dir arg */

echo "*** Testing tempnam() with relative paths ***\n";
$dir_name = dirname(__FILE__)."/tempnam_variation2";
mkdir($dir_name);
$dir_path = $dir_name."/tempnam_variation2_sub";
mkdir($dir_path);

$old_dir_path = getcwd();
chdir(dirname(__FILE__));

$dir_paths = array(
  "$dir_path",
  "$dir_path/",
  "$dir_path/..",
  "$dir_path//../",
  "$dir_path/../.././tempnam_variation2",
  "$dir_path/..///tempnam_variation2_sub//..//../tempnam_variation2",
  ".",
  "./tempnam_variation2/../tempnam_variation2/tempnam_variation2_sub"
);

for($i = 0; $i<count($dir_paths); $i++) {
  $j = $i+1;
  echo "\n-- Iteration $j --\n";
  $file_name = tempnam($dir_paths[$i], "tempnam_variation2.tmp");
  
  if( file_exists($file_name) ){

    echo "File name is => ";
    print(realpath($file_name));
    echo "\n";

    echo "File permissions are => ";
    printf("%o", fileperms($file_name) );
    echo "\n";
  }
  else
    echo "-- File is not created --";
    unlink($file_name);
}

chdir($old_dir_path);
rmdir($dir_path);
rmdir($dir_name);

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing tempnam() with relative paths ***

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
File name is => %s
File permissions are => 100600

-- Iteration 8 --
File name is => %s
File permissions are => 100600

*** Done ***
