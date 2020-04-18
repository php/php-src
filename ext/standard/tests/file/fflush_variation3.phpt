--TEST--
Test fflush() function: usage variations - hard links as resource
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN')
  die("skip Links not valid on Windows");
?>
--FILE--
<?php
/*  Prototype: bool fflush ( resource $handle );
    Description: Flushes the output to a file
*/

/* test fflush() with handle to hard links as resource */

$file_path = __DIR__;
require $file_path.'/file.inc';

echo "*** Testing fflush(): with hard links to files opened in diff modes ***\n";
$file_types = array("empty", "numeric", "text", "text_with_new_line", "alphanumeric");
$file_modes = array("w", "wb", "wt", "w+", "w+b","w+t",
                    "a", "ab", "at", "a+","a+b", "a+t");

$file_name = "$file_path/fflush_variation3.tmp";
$link_name = "$file_path/lnk_fflush_variation3.tmp";

$count = 1;

foreach( $file_types as $type ) {
  echo "-- Iteration $count with file containing $type data --\n";
  foreach( $file_modes as $mode ) {

    // creating the file
    $file_handle = fopen($file_name, "w");
    if($file_handle == false)
      exit("Error:failed to open file $file_name");

    // fill the fill with some data if mode is append mode
    if( substr($mode, 0, 1) == "a" )
      fill_file($file_handle, $type, 10);

    // fclose($file_handle);

    // creating hard link to the file
    var_dump( link($file_name, $link_name) );

    // opening the file in different modes
    $file_handle = fopen($link_name, $mode);
    if($file_handle == false)
      exit("Error:failed to open link $link_name");

    // writing data to the file
    var_dump( fill_file($file_handle, $type, 50) );
    var_dump( fflush($file_handle) );
    fclose($file_handle);

    // reading data from the file after flushing
    var_dump( readfile($link_name) );

    unlink($link_name);
    unlink($file_name);
  }
  $count++;
}

echo "\n*** Done ***";
?>
--EXPECT--
*** Testing fflush(): with hard links to files opened in diff modes ***
-- Iteration 1 with file containing empty data --
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(0)
-- Iteration 2 with file containing numeric data --
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- Iteration 3 with file containing text data --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- Iteration 4 with file containing text_with_new_line data --
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
-- Iteration 5 with file containing alphanumeric data --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)

*** Done ***
