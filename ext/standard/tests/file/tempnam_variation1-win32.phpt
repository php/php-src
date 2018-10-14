--TEST--
Test tempnam() function: usage variations - creating files
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Only valid for Windows");
?>
--FILE--
<?php
/* Prototype:  string tempnam ( string $dir, string $prefix );
   Description: Create file with unique file name.
*/

/* Creating number of unique files by passing a file name as prefix */

$file_path = dirname(__FILE__)."/tempnamVar1";
mkdir($file_path);

echo "*** Testing tempnam() in creation of unique files ***\n";
for($i=1; $i<=10; $i++) {
  echo "-- Iteration $i --\n";
  $files[$i] = tempnam("$file_path", "tempnam_variation1.tmp");

  if( file_exists($files[$i]) ) {

    echo "File name is => ";
    print($files[$i]);
    echo "\n";

    echo "File permissions are => ";
    printf("%o", fileperms($files[$i]) );
    echo "\n";
    clearstatcache();

    echo "File created in => ";
    $file_dir = dirname($files[$i]);

    if (realpath($file_dir) == realpath(sys_get_temp_dir()) || realpath($file_dir."\\") == realpath(sys_get_temp_dir())) {
       echo "temp dir\n";
    }
    else if (realpath($file_dir) == realpath($file_path) || realpath($file_dir."\\") == realpath($file_path)) {
       echo "directory specified\n";
    }
    else {
       echo "unknown location\n";
    }
    clearstatcache();
  }
  else {
    print("- File is not created -");
  }
}
for($i=1; $i<=10; $i++) {
  unlink($files[$i]);
}
rmdir($file_path);


echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing tempnam() in creation of unique files ***
-- Iteration 1 --
File name is => %s%et%s
File permissions are => 100666
File created in => directory specified
-- Iteration 2 --
File name is => %s%et%s
File permissions are => 100666
File created in => directory specified
-- Iteration 3 --
File name is => %s%et%s
File permissions are => 100666
File created in => directory specified
-- Iteration 4 --
File name is => %s%et%s
File permissions are => 100666
File created in => directory specified
-- Iteration 5 --
File name is => %s%et%s
File permissions are => 100666
File created in => directory specified
-- Iteration 6 --
File name is => %s%et%s
File permissions are => 100666
File created in => directory specified
-- Iteration 7 --
File name is => %s%et%s
File permissions are => 100666
File created in => directory specified
-- Iteration 8 --
File name is => %s%et%s
File permissions are => 100666
File created in => directory specified
-- Iteration 9 --
File name is => %s%et%s
File permissions are => 100666
File created in => directory specified
-- Iteration 10 --
File name is => %s%et%s
File permissions are => 100666
File created in => directory specified
*** Done ***
