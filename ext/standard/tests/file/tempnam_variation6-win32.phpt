--TEST--
Test tempnam() function: usage variations - Using previous unique filename
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Windows Only");
?>
--FILE--
<?php
/* Trying to create unique files by passing previously created unique file name as prefix */

$file_path = __DIR__;

echo "\n*** Test tempnam(): by passing previously created filenames ***\n";
$file_name = "tempnam_variation6.tmp";
for($i=1; $i<=3; $i++) {
  echo "-- Iteration $i --\n";
  $file_name = tempnam("$file_path", $file_name);

  if( file_exists($file_name) ) {
    echo "File name is => ";
    print($file_name);
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

  unlink($file_name);
}

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Test tempnam(): by passing previously created filenames ***
-- Iteration 1 --
File name is => %s%et%s
File created in => directory specified
-- Iteration 2 --
File name is => %s%et%s
File created in => directory specified
-- Iteration 3 --
File name is => %s%et%s
File created in => directory specified

*** Done ***
